
#ifndef PING_H_
#define PING_H_

#include <stdint.h>
#include <stdbool.h>
#include <inc/tm4c123gh6pm.h>
#include "driverlib/interrupt.h"

//extern volatile uint32_t g_start_time;       // Rising edge timestamp
//extern volatile uint32_t g_end_time;         // Falling edge timestamp
//typedef enum { LOW, HIGH, DONE } ping_state;
//extern volatile ping_state g_state;          // State of ping echo pulse
//extern volatile uint32_t g_overflow_count;   // Counts timer overflows

/**
 * Initialize ping sensor. Uses PB3 and Timer 3B
 */
void ping_init (void);

/**
 * @brief Trigger the ping sensor
 */
void ping_trigger (void);

/**
 * @brief Timer3B ping ISR
 */
void TIMER3B_Handler(void);

/**
 * @brief Calculate the distance in meters
 *
 * @return Distance in meters
 */
float ping_getDistance (void);

#endif /* PING_H_ */
