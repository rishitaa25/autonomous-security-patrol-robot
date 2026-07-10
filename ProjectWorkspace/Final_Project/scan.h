#ifndef SCAN_H_
#define SCAN_H_

#include <stdint.h>

// Feature enable bits
#define CYBOT_SERVO_EN  0x01
#define CYBOT_PING_EN   0x02
#define CYBOT_IR_EN     0x04

// Scan result
typedef struct {
    float sound_dist;   // Distance in meters from PING sensor (-1.0 if PING not enabled)
    int   IR_raw_val;   // Raw ADC value from IR sensor (-1 if IR not enabled)
} cyBOT_Scan_t;

// Calibration struct
typedef struct {
    int right;  // 0-degree (right) calibration pulse value
    int left;   // 180-degree (left) calibration pulse value
} cyBOT_SERVO_cal_t;

// Global calibration values — assign after calling cyBOT_SERVO_cal()
extern int right_calibration_value;
extern int left_calibration_value;

// Initialize selected features (OR together CYBOT_*_EN bits)
// Example: cyBOT_init_Scan(CYBOT_SERVO_EN | CYBOT_PING_EN | CYBOT_IR_EN);
void cyBOT_init_Scan(int features);

// Point sensors to angle and read measurement
// angle: 0–180 degrees
// getScan: pointer to a cyBOT_Scan_t to fill
void cyBOT_Scan(int angle, cyBOT_Scan_t *getScan);

// Run interactive servo calibration, returns min/max pulse values
// NOTE: returns {-1, -1} if servo was not enabled
cyBOT_SERVO_cal_t cyBOT_SERVO_cal(void);

#endif /* SCAN_H_ */