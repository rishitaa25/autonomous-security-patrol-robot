

#ifndef MOVEMENT_H_
#define MOVEMENT_H_

#include "open_interface.h"
#include <stdbool.h>
#include <stdint.h>
#include "detect.h"

//
//#define LEFT_TAPE_THRESHOLD         2710
//#define FRONT_LEFT_TAPE_THRESHOLD   2730
//#define RIGHT_TAPE_THRESHOLD        1250
//#define FRONT_RIGHT_TAPE_THRESHOLD  2650
//
//#define LEFT_HOLE_THRESHOLD         650
//#define FRONT_LEFT_HOLE_THRESHOLD   1200
//#define FRONT_RIGHT_HOLE_THRESHOLD  1200
//#define RIGHT_HOLE_THRESHOLD        900

//----CYBOT21----
//#define LEFT_TAPE_THRESHOLD         2775
//#define FRONT_LEFT_TAPE_THRESHOLD   1600
//#define RIGHT_TAPE_THRESHOLD        2725
//#define FRONT_RIGHT_TAPE_THRESHOLD  2815
//
//#define LEFT_HOLE_THRESHOLD         75
//#define FRONT_LEFT_HOLE_THRESHOLD   25
//#define FRONT_RIGHT_HOLE_THRESHOLD  115
//#define RIGHT_HOLE_THRESHOLD        190

//----CYBOT21----
#define LEFT_TAPE_THRESHOLD         2775
#define FRONT_LEFT_TAPE_THRESHOLD   2770
#define RIGHT_TAPE_THRESHOLD        2800
#define FRONT_RIGHT_TAPE_THRESHOLD  2750

#define LEFT_HOLE_THRESHOLD         75
#define FRONT_LEFT_HOLE_THRESHOLD   25
#define FRONT_RIGHT_HOLE_THRESHOLD  115
#define RIGHT_HOLE_THRESHOLD        190

// ---------------------------------------------------------------
// Thresholds  Bot 06
// ---------------------------------------------------------------
//#define LEFT_TAPE_THRESHOLD         1974
//#define FRONT_LEFT_TAPE_THRESHOLD   2572
//#define RIGHT_TAPE_THRESHOLD        2288
//#define FRONT_RIGHT_TAPE_THRESHOLD  2201
//
//#define LEFT_HOLE_THRESHOLD         650
//#define FRONT_LEFT_HOLE_THRESHOLD   1200
//#define FRONT_RIGHT_HOLE_THRESHOLD  1200
//#define RIGHT_HOLE_THRESHOLD        900

// ---------------------------------------------------------------
// Speed / Distance constants
// ---------------------------------------------------------------
#define FORWARD_SPEED_R               80
#define FORWARD_SPEED_L               80
#define TURN_SPEED                  50
#define STEP_MM                     500
#define BACKUP_MM                   1500

// ---------------------------------------------------------------
// Object detection thresholds — tune during testing
// ---------------------------------------------------------------
#define IR_THRESHOLD                750
#define MIN_LINEAR_WIDTH_CM         4.0f
#define OBSTACLE_CLEAR_CM           35.0f
#define INTRUDER_MIN_WIDTH_CM       10.0f
#define SCAN_STEP_DEG               2


// ---------------------------------------------------------------
// Interrupt flags
// Cliff/bump flags are set by ISRs
// Main loop reads and clears them
// ---------------------------------------------------------------
extern volatile bool flag_bump;
extern volatile bool flag_hole;
extern volatile bool flag_front_tape;
extern volatile bool flag_left_tape;
extern volatile bool flag_manual;
extern volatile char manual_cmd;

// Bump sides saved by ISR since oi_t not safe to read in ISR
extern volatile bool isr_bump_left;
extern volatile bool isr_bump_right;

// ---------------------------------------------------------------
// ISR prototypes
// Register these with your interrupt controller in main.c
// Cliff/tape/hole ISR reads directly from oi_t sensor_data
// ---------------------------------------------------------------
void cliff_sensor_isr(oi_t *sensor_data);  // pass sensor_data so ISR
                                            // reads oi_t cliff signals
void bump_sensor_isr(oi_t *sensor_data);   // same for bump
void uart_rx_isr(char received);

// ---------------------------------------------------------------
// Tape / hole detection helpers
// These read directly from oi_t — call after oi_update()
// ---------------------------------------------------------------
bool left_on_tape(oi_t *sensor_data);
bool front_on_tape(oi_t *sensor_data);
bool right_on_tape(oi_t *sensor_data);
bool hole_detected(oi_t *sensor_data);
bool bump_detected(oi_t *sensor_data);

// ---------------------------------------------------------------
// Movement primitives
// ---------------------------------------------------------------
void   stop_robot(void);
double move_forward(oi_t *sensor_data, double distance_mm);
double move_backward(oi_t *sensor_data, double distance_mm);
double turn_left(oi_t *sensor_data, double degrees);
double turn_right(oi_t *sensor_data, double degrees);

// ---------------------------------------------------------------
// Object detection
// ---------------------------------------------------------------
object_t scan_objects(int scan_angle1, int scan_angle2);
void     turn_to_object(oi_t *sensor_data, object_t obj);
void     investigate_and_deter(oi_t *sensor_data, object_t intruder);

// ---------------------------------------------------------------
// Interrupt handlers
// ---------------------------------------------------------------
void handle_manual_command(oi_t *sensor_data);
void handle_hole(oi_t *sensor_data);
void handle_bump(oi_t *sensor_data);
void handle_corner(oi_t *sensor_data);
void handle_left_tape(oi_t *sensor_data);

// ---------------------------------------------------------------
// Patrol
// ---------------------------------------------------------------
bool scan_then_step(oi_t *sensor_data);
void align_from_corner(oi_t *sensor_data);
void follow_boundary_counterclockwise(oi_t *sensor_data);
void run_security_patrol(oi_t *sensor_data);

#endif /* MOVEMENT_H_ */
