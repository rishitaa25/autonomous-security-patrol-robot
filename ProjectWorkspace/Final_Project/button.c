
//The buttons are on PORTE 3:0
// GPIO_PORTE_DATA_R -- Name of the memory mapped register for GPIO Port E,
// which is connected to the push buttons
#include "button.h"


/**
 * Initialize PORTE and configure bits 0-3 to be used as inputs for the buttons.
 */
void button_init() {
    static uint8_t initialized = 0;

    //Check if already initialized
    if(initialized){
        return;
    }

    // Reading: To initialize and configure GPIO PORTE, visit pg. 656 in the
    // Tiva datasheet.

    // Follow steps in 10.3 for initialization and configuration. Some steps
    // have been outlined below.

    // Ignore all other steps in initialization and configuration that are not
    // listed below. You will learn more about additional steps in a later lab.

    // 1) Turn on PORTE system clock, do not modify other clock enables
        SYSCTL_RCGCGPIO_R |= 0x10;

    // You may need to add a delay here of several clock cycles for the clock to start, e.g., execute a
    // simple dummy assignment statement, such as "long delay = SYSCTL_RCGCGPIO_R".
    // Instead, use the PRGPIO register and busy-wait on the peripheral ready bit for PORTE.
        while ((SYSCTL_PRGPIO_R & 0x10) == 0) {};

    // 2) Set the buttons as inputs, do not modify other PORTE wires
        GPIO_PORTE_DIR_R &= ~0x0F;

    // 3) Enable digital functionality for button inputs,
    //    do not modify other PORTE enables
        GPIO_PORTE_DEN_R |= 0x0F;
        GPIO_PORTE_PUR_R |= 0x0F;


    initialized = 1;
}



/**
 * Returns the position of the rightmost button being pushed.
 * @return the position of the rightmost button being pushed. 1 is the leftmost button, 4 is the rightmost button.  0 indicates no button being pressed
 */
uint8_t button_getButton() {


    uint8_t buttons = ~GPIO_PORTE_DATA_R & 0x0F;

        // Return the rightmost button position pressed
        switch (buttons) {
            case 0x08: return 4;  // PE3 pressed 1000
            case 0x04: return 3;  // PE2 pressed 0100
            case 0x02: return 2;  // PE1 pressed 0010
            case 0x01: return 1;  // PE0 pressed 0001
            default:return 0;  // no button pressed
        }
}
