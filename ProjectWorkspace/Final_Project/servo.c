/**
 * Driver for Servo
 * @file servo.c
 * @author
 *
 * This driver handles sending the trigger pulse to the PING)))
 * sensor and measuring the echo pulse using Timer 3B in edge-time
 * capture mode. Distance is calculated based on timer ticks.
 */

#include "servo.h"
#include "Timer.h"
#include "lcd.h"
#include "button.h"
#include <stdio.h>

uint32_t min_pulse = 6000;   // Will become calibrated 0°
uint32_t max_pulse = 40000;   // Will become calibrated 180°
uint32_t current_pulse = 24000; // Start at center

// Initializes Timer1B and PB5 for PWM signal generation
void servo_init(void){
    SYSCTL_RCGCTIMER_R |= 0x02;   // Enable clock for Timer1
    SYSCTL_RCGCGPIO_R |= 0x02;    // Enable clock for Port B

    while((SYSCTL_PRGPIO_R & 0x02) == 0){}; // Wait for Port B ready

    GPIO_PORTB_DIR_R |= 0x20;     // Set PB5 as output
    GPIO_PORTB_DEN_R |= 0x20;     // Enable digital function on PB5
    GPIO_PORTB_AFSEL_R |= 0x20;   // Enable alternate function on PB5

    GPIO_PORTB_PCTL_R &= ~0xF00000; // Clear PCTL bits for PB5
    GPIO_PORTB_PCTL_R |=  0x700000; // Set PB5 to T1CCP1 (Timer1B)

    TIMER1_CTL_R &= ~0x0100;      // Disable Timer1B during setup
    TIMER1_CFG_R = 0x4;           // Configure as 16-bit timer
    TIMER1_TBMR_R = 0x0A;         // PWM mode, periodic, down-count

    TIMER1_TBILR_R = 320000 & 0xFFFF;      // Set 20 ms period (lower bits)
    TIMER1_TBPR_R = (320000 >> 16) & 0xFF; // Set period (upper bits)

    TIMER1_TBMATCHR_R = (320000 - 24000) & 0xFFFF; // 1.5 ms pulse (center)
    TIMER1_TBPMR_R = ((320000 - 24000) >> 16) & 0xFF;

    TIMER1_CTL_R |= 0x0100;       // Enable Timer1B
}

// Moves servo to a specified angle (0ï¿½180 degrees)
void servo_move(uint16_t degrees){
    if(degrees > 180) degrees = 180; // Limit max angle

    //uint32_t pulse = 16000 + (degrees * 16000 / 180); // Convert angle to pulse width 
    uint32_t pulse = min_pulse + (degrees * (max_pulse - min_pulse) / 180); //Use with calibration functino

    uint32_t match = 320000 - pulse; // Convert to match value

    TIMER1_TBMATCHR_R = match & 0xFFFF;      // Set lower match value
    TIMER1_TBPMR_R = (match >> 16) & 0xFF;   // Set upper match value
}

void servo_set_pulse(uint32_t pulse){
    if(pulse < 6000) pulse = 6000;   // safety clamp
    if(pulse > 40000) pulse = 40000;

    current_pulse = pulse;

    uint32_t match = 320000 - pulse;

    TIMER1_TBMATCHR_R = match & 0xFFFF;
    TIMER1_TBPMR_R = (match >> 16) & 0xFF;
}

void servo_calibrate(void){
    servo_set_pulse(current_pulse);
    
    lcd_clear();
    lcd_printf("Calibrate Servo\nPB1<-\nPB2->\nPB4=Set");

    int step = 0; // 0 = set min, 1 = set max
    char buffer[20];

    while(step < 2){
        int btn = button_getButton();

        if(btn != 0){
            timer_waitMillis(20);
        }

        if(btn == 1){ // PB1 left
            current_pulse += 100;
        }
        else if(btn == 2){ // PB2 right
            current_pulse -= 100;
        }
        else if(btn == 4){ // PB4 save
            if(step == 0){
                min_pulse = current_pulse;
                lcd_clear();
                lcd_printf("Saved 0 deg\nNow set 180");
                timer_waitMillis(1000);
                step++;
            }
            else{
                max_pulse = current_pulse;
                lcd_clear();
                lcd_printf("Saved 180 deg");
                timer_waitMillis(1000);
                step++;
            }
        }

        // Clamp values
        if(current_pulse < 6000) current_pulse = 6000;
        if(current_pulse > 40000) current_pulse = 40000;

        // Apply movement AFTER updating
        servo_set_pulse(current_pulse);

        timer_waitMillis(15);
    }

    lcd_clear();
    lcd_printf("Calibration Done!");
    sprintf(buffer, "Pulse:%lu", current_pulse);
    lcd_printf("%s\nMin:%lu\nMax:%lu", buffer, min_pulse, max_pulse);
    timer_waitMillis(1500);
}
