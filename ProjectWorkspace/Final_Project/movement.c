/*
 * movement.c
 *
 *  Created on: Feb 4, 2026
 *      Author: Rishita Kollu, Lavanya Vangapandu, Kaylee Johnson, Olivia Blais
 */
#include "open_interface.h"
#include "movement.h"
#include "Timer.h"
#include "lcd.h"
#include "scan.h"  // was "scan.h" — fixed
#include "uart.h"
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include "alarm.h"

/*
*  Threshold configuration
 *
 * Each sensor has a hole threshold and a tape threshold.
 * Tune these values to match your floor/tape calibration.
 *
 * Replace the placeholder values below with your actual #defines
 * from movement.h, or just set them here directly.
 */
typedef struct {
    uint16_t hole;   // signal BELOW this -> hole/cliff
    uint16_t tape;   // signal ABOVE this -> tape detected
} SensorThreshold;

static const SensorThreshold THRESH_LEFT        = {.hole = LEFT_HOLE_THRESHOLD, .tape = LEFT_TAPE_THRESHOLD};
static const SensorThreshold THRESH_FRONT_LEFT  = {.hole = FRONT_LEFT_HOLE_THRESHOLD, .tape = FRONT_LEFT_TAPE_THRESHOLD};
static const SensorThreshold THRESH_FRONT_RIGHT = {.hole = FRONT_RIGHT_HOLE_THRESHOLD, .tape = FRONT_RIGHT_TAPE_THRESHOLD};
static const SensorThreshold THRESH_RIGHT       = {.hole = RIGHT_HOLE_THRESHOLD, .tape = RIGHT_TAPE_THRESHOLD};

/*  Interrupt flags
 *
 * Grouped into a single struct so all cliff state lives in one place.
 * Reset between runs with:  cliff = (CliffState){0};
 */
typedef struct {
    bool hole;
    bool tape_left;
    bool tape_front_left;
    bool tape_front_right;
    bool tape_right;
} CliffState;

volatile CliffState cliff = {0};

volatile bool flag_manual     = false;
volatile char manual_cmd      = 0;

volatile bool isr_bump_left   = false;
volatile bool isr_bump_right  = false;

/*  Internal signal helpers
 *
 * Named predicates so the ISR reads like plain English.
 * The digital_cliff bit is the hardware's own built-in detection —
 * we check both it AND the raw signal for redundancy.
 */
static inline bool signal_is_hole(uint16_t signal, SensorThreshold t, bool digital_cliff) {
    return (signal < t.hole) || digital_cliff;
}

static inline bool signal_is_tape(uint16_t signal, SensorThreshold t) {
    return signal > t.tape;
}

/*
 * ISRs
 *
 * cliff_sensor_isr now takes sensor_data so it can read
 * directly from oi_t fields — oi_get_cliff_*() do not exist
 * in open_interface.h so we removed those calls entirely.
 *
 * Call oi_update(sensor_data) BEFORE calling this ISR so
 * the struct has fresh values.
 *
 * Reads cliff signals and updates the global `cliff` state struct.
 * Hole detection takes full priority — tape flags are skipped if any
 * sensor sees a cliff.
 *
 */
void cliff_sensor_isr(oi_t *sensor_data) {

    uint16_t sig_l  = sensor_data->cliffLeftSignal;
    uint16_t sig_fl = sensor_data->cliffFrontLeftSignal;
    uint16_t sig_fr = sensor_data->cliffFrontRightSignal;
    uint16_t sig_r  = sensor_data->cliffRightSignal;

    // Hole check — if anything sees a cliff, stop and bail out
    if (signal_is_hole(sig_l,  THRESH_LEFT,        sensor_data->cliffLeft)       ||
        signal_is_hole(sig_fl, THRESH_FRONT_LEFT,  sensor_data->cliffFrontLeft)  ||
        signal_is_hole(sig_fr, THRESH_FRONT_RIGHT, sensor_data->cliffFrontRight) ||
        signal_is_hole(sig_r,  THRESH_RIGHT,       sensor_data->cliffRight))
    {
        cliff.hole = true;
        return;
    }

    // Tape check — only reached when no hole is present
    cliff.tape_left        = signal_is_tape(sig_l,  THRESH_LEFT);
    cliff.tape_front_left  = signal_is_tape(sig_fl, THRESH_FRONT_LEFT);
    cliff.tape_front_right = signal_is_tape(sig_fr, THRESH_FRONT_RIGHT);
    cliff.tape_right       = signal_is_tape(sig_r,  THRESH_RIGHT);
}

/*
*  Public tape polling API
 *
 * Use these for on-demand checks outside the ISR (e.g. in movement loops).
 * They read directly from the sensor struct rather than cached flags.
 */
bool cliff_left_on_tape(oi_t *s) {
     return signal_is_tape(s->cliffLeftSignal, THRESH_LEFT);
 }

bool cliff_front_left_on_tape(oi_t *s) {
    return signal_is_tape(s->cliffFrontLeftSignal, THRESH_FRONT_LEFT);
}

bool cliff_front_right_on_tape(oi_t *s) {
    return signal_is_tape(s->cliffFrontRightSignal, THRESH_FRONT_RIGHT);
}

bool cliff_right_on_tape(oi_t *s) {
    return signal_is_tape(s->cliffRightSignal, THRESH_RIGHT);
}

/*  Movement
 *
 * All movement functions return the actual distance/angle traveled.
 * stop_robot() is always called before returning.
 */
void stop_robot(void) {
    oi_setWheels(0, 0);
}

double move_forward(oi_t *sensor_data, double distance_mm) {
    double distance_traveled = 0;
    int bumpLeft = 0;
    int bumpRight = 0;

    oi_setWheels(FORWARD_SPEED_R, FORWARD_SPEED_L);

    while (distance_traveled < distance_mm) {
        oi_update(sensor_data);
        distance_traveled += sensor_data->distance;

        lcd_printf("D:%.0f\nBL:%d BR:%d", distance_traveled, sensor_data->bumpLeft, sensor_data->bumpRight);

        bumpLeft  = sensor_data->bumpLeft;
        bumpRight = sensor_data->bumpRight;
        if (bumpLeft || bumpRight){
            stop_robot();
            move_backward(sensor_data, 50);

            uart_sendStr("BUMP DETECTED\n");
            alertBelow();
            char detect[50];
                       sprintf(detect, "RIGHT: %d\n LEFT: %d \r\n",
                                       bumpRight, bumpLeft );
                       uart_sendStr(detect);

            return distance_traveled - 50;
        }

        if (cliff_front_right_on_tape(sensor_data) || cliff_right_on_tape(sensor_data) ||
                cliff_front_left_on_tape(sensor_data) || cliff_left_on_tape(sensor_data))  {
            stop_robot();

            uart_sendStr("TAPE DETECTED\n");
            alertBelow();
            char detect[50];
            sprintf(detect, "FRONT RIGHT: %d\n RIGHT: %d\n FRONT LEFT: %d\n LEFT: %d \r\n",
                            cliff_front_right_on_tape(sensor_data), cliff_right_on_tape(sensor_data), cliff_front_left_on_tape(sensor_data), cliff_left_on_tape(sensor_data));
            uart_sendStr(detect);

            return distance_traveled;
        }

       if (sensor_data-> cliffRight || sensor_data-> cliffFrontRight || sensor_data-> cliffFrontLeft || sensor_data-> cliffLeft){
           stop_robot();

           uart_sendStr("HOLE DETECTED\n");
           alertBelow();
           char detect[50];
           sprintf(detect, "FRONT RIGHT: %d\n RIGHT: %d\n FRONT LEFT: %d\n LEFT: %d \r\n",
                           sensor_data-> cliffFrontRight, sensor_data-> cliffRight, sensor_data-> cliffFrontLeft, sensor_data-> cliffLeft);
           uart_sendStr(detect);

           return distance_traveled;
       }



    }

//    turn_right(sensor_data, 10);
    stop_robot();
    return distance_traveled;
}

double move_backward(oi_t *sensor_data, double distance_mm) {
    double distance_traveled = 0;

    oi_setWheels(-FORWARD_SPEED_R, -FORWARD_SPEED_L);

    while (distance_traveled > -distance_mm) {
        oi_update(sensor_data);
        distance_traveled += sensor_data->distance;
    }
//    turn_left(sensor_data, 10);

    stop_robot();
    return distance_traveled;
}

double turn_left(oi_t *sensor_data, double degrees) {
    double angle = 0;

    oi_setWheels(TURN_SPEED, -TURN_SPEED);

    while (angle < degrees) {
        if (flag_manual) {
            stop_robot();
            return angle;
        }
        oi_update(sensor_data);
        angle += sensor_data->angle;
    }

    stop_robot();
    return angle;
}

double turn_right(oi_t *sensor_data, double degrees) {
    double angle = 0;

    oi_setWheels(-TURN_SPEED, TURN_SPEED);

    while (angle > -degrees) {
        oi_update(sensor_data);
        angle += sensor_data->angle;
    }

    stop_robot();
    return angle;
}




