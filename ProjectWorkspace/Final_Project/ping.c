

#include "ping.h"
#include "Timer.h"

// Global shared variables
// Use extern declarations in the header file

volatile uint32_t g_start_time = 0;
volatile uint32_t g_end_time = 0;
volatile enum{LOW, HIGH, DONE} g_state = LOW; // State of ping echo pulse
volatile uint32_t g_overflow_count = 0;

void ping_init (void){

    SYSCTL_RCGCGPIO_R |= 0x02;
    SYSCTL_RCGCTIMER_R |= 0x08;



    while((SYSCTL_PRGPIO_R & 0x02) == 0){}

    GPIO_PORTB_DIR_R &= ~0x08;
    GPIO_PORTB_AFSEL_R |= 0x08;


    GPIO_PORTB_PCTL_R &= ~0x0000F000;
    GPIO_PORTB_PCTL_R |= 0x00007000;

    GPIO_PORTB_DEN_R |= 0x08;


    //Disable timer to setup
    TIMER3_CTL_R &= ~0x0100;

    //disable interrupt
    TIMER3_IMR_R &= ~0x0400;

    //configure Timer3B
    TIMER3_CFG_R = 0x04;

    //input edge-time mode
    TIMER3_TBMR_R = (TIMER3_TBMR_R & ~0x03) | 0x03;


    TIMER3_TBMR_R |= 0x04;

    //countdown mode
    TIMER3_TBMR_R &= ~0x10;

    //capture both edges
    TIMER3_CTL_R |= 0x0C00;

    TIMER3_TBPR_R = 0xFF;
    TIMER3_TBILR_R = 0xFFFF;

    TIMER3_IMR_R |= 0x0400;


    IntRegister(INT_TIMER3B, TIMER3B_Handler);

    NVIC_EN1_R |= 0x10;

    //IntEnable(INT_TIMER3B);
    IntMasterEnable();

    // Configure and enable the timer
    TIMER3_CTL_R |= 0x0100;
}

void ping_trigger (void){
    g_state = LOW;
    // Disable timer and disable timer interrupt
    TIMER3_CTL_R &= ~0x0100;
    TIMER3_IMR_R &= ~0x0400;
    // Disable alternate function (disconnect timer from port pin)
    GPIO_PORTB_AFSEL_R &= ~0x08;

    GPIO_PORTB_DIR_R |= 0x08;
    GPIO_PORTB_DEN_R |= 0x08;


    // YOUR CODE HERE FOR PING TRIGGER/START PULSE
    GPIO_PORTB_DATA_R &= ~0x08; //low PB3
    GPIO_PORTB_DATA_R |= 0x08;

    //for(int i=0; i<10; i++)[]; //delay

   // for(int i=0; i<80; i++)[]; //5 ms

    timer_waitMicros(5);

    GPIO_PORTB_DATA_R &= ~0x08;

    // Clear an interrupt that may have been erroneously triggered
    TIMER3_ICR_R = 0x0400;
    // Re-enable alternate function, timer interrupt, and timer
    GPIO_PORTB_AFSEL_R  |= 0x08;
    TIMER3_IMR_R |= 0x0400;
    TIMER3_CTL_R |= 0x0100;
}

void TIMER3B_Handler(void){

  // YOUR CODE HERE
  // As needed, go back to review your interrupt handler code for the UART lab.
  // What are the first lines of code in the ISR? Regardless of the device, interrupt handling
  // includes checking the source of the interrupt and clearing the interrupt status bit.
  // Checking the source: test the MIS bit in the MIS register (is the ISR executing
  // because the input capture event happened and interrupts were enabled for that event?
  // Clearing the interrupt: set the ICR bit (so that same event doesn't trigger another interrupt)
  // The rest of the code in the ISR depends on actions needed when the event happens.

    if(TIMER3_MIS_R & 0x0400){

        TIMER3_ICR_R = 0x0400;

        if(g_state == LOW){
            g_start_time = TIMER3_TBR_R;
            g_state = HIGH;
        }else if(g_state == HIGH){
            g_end_time = TIMER3_TBR_R;
            g_state = DONE;
        }
    }

}

/*float ping_getDistance(void){

    uint32_t pulse_width;
    float time_us;
    float distance_cm;

    while(g_state != DONE){}

    if(g_start_time < g_end_time){

        pulse_width = (g_start_time + (0xFFFFFF - g_end_time));
        g_overflow_count++;

    }else{
        pulse_width = g_start_time - g_end_time;
    }

    time_us = pulse_width * 0.0625;

    distance_cm = time_us/58.0;
    return distance_cm;
}*/

// -------------------- Convert timer ticks to distance (meters) --------------------
float ping_getDistance(void) {
    if (g_state != DONE) return 0.0f;   // measurement not complete

    uint32_t ticks;

    // Handle wrap-around in count-down mode (16-bit timer + 8-bit prescaler = 24-bit)
    if (g_start_time >= g_end_time) {
        ticks = g_start_time - g_end_time;                   // normal case
    } else {
        ticks = g_start_time + (0xFFFFFF - g_end_time) + 1; // wrapped around
    }

    // Add full 24-bit timer periods if overflow occurred
    ticks += g_overflow_count * 0x1000000; // 2^24 = 16,777,216 ticks per overflow

    // Convert ticks to seconds (16 MHz timer)
    float time_sec = ticks / 16000000.0f;

    // Convert to distance in meters (speed of sound 343 m/s, divide by 2 for round trip)
    float distance_m = (time_sec * 343.0f) / 2.0f;

    return distance_m;
}
