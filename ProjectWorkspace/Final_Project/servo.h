
#ifndef SERVO_H_
#define SERVO_H_

#include <stdint.h>
#include <stdbool.h>
#include <inc/tm4c123gh6pm.h>
#include "driverlib/interrupt.h"

extern uint32_t min_pulse;
extern uint32_t max_pulse;

void servo_init(void);

void servo_move(uint16_t degrees);

void servo_set_pulse(uint32_t pulse);

void servo_calibrate(void);

#endif /* SERVO_H_ */
