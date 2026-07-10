#include "scan.h"
#include "servo.h"
#include "ping.h"
#include "adc.h"
#include "Timer.h"

// Global calibration values
int right_calibration_value = 0;
int left_calibration_value  = 0;

// Track which features are active
static int enabled_features = 0;

void cyBOT_init_Scan(int features) {
    enabled_features = features;

    if (features & CYBOT_SERVO_EN) {
        servo_init();
    }
    if (features & CYBOT_PING_EN) {
        ping_init();
    }
    if (features & CYBOT_IR_EN) {
        adc_init();
    }
}

void cyBOT_Scan(int angle, cyBOT_Scan_t *getScan) {
    // Default return values
    getScan->sound_dist = -1.0f;
    getScan->IR_raw_val = -1;

    // Move servo if enabled
    if (enabled_features & CYBOT_SERVO_EN) {
        // Use calibrated range if values have been set
        if (right_calibration_value != 0 && left_calibration_value != 0) {
            uint32_t pulse = right_calibration_value +
                             (angle * (left_calibration_value - right_calibration_value) / 180);
            servo_set_pulse(pulse);
        } else {
            servo_move((uint16_t)angle);
        }
        timer_waitMillis(150); // Let servo settle before measuring
    }

    // PING measurement
    if (enabled_features & CYBOT_PING_EN) {
        ping_trigger();
        timer_waitMillis(30); // Wait for echo to complete
        getScan->sound_dist = ping_getDistance();
    }

    // IR measurement
    if (enabled_features & CYBOT_IR_EN) {
        getScan->IR_raw_val = (int)adc_read();
    }
}

cyBOT_SERVO_cal_t cyBOT_SERVO_cal(void) {
    cyBOT_SERVO_cal_t cal = { -1, -1 };

    if (!(enabled_features & CYBOT_SERVO_EN)) {
        return cal;
    }

    // servo_calibrate() sets min_pulse (0°) and max_pulse (180°)
    // These are defined as globals in servo.c — expose them via servo.h
    servo_calibrate();

    // Pull the calibrated values back out
    // (Requires servo.h to expose: extern uint32_t min_pulse, max_pulse;)
    extern uint32_t min_pulse;
    extern uint32_t max_pulse;

    cal.right = (int)min_pulse;
    cal.left  = (int)max_pulse;

    right_calibration_value = cal.right;
    left_calibration_value  = cal.left;

    return cal;
}
