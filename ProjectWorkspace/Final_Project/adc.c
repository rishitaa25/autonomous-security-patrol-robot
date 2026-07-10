
#include "adc.h"
#include <math.h>


void adc_init(void)
{
    SYSCTL_RCGCADC_R |= 0x01;     // Enable ADC0 clock
    SYSCTL_RCGCGPIO_R |= 0x02;    // Enable Port B
    while((SYSCTL_PRGPIO_R & 0x02) == 0){}; // Wait for Port B
    while((SYSCTL_PRADC_R & 0x01) == 0){}; // Wait for ADC
    GPIO_PORTB_DIR_R &= ~0x10;    // Make PB4 input
    GPIO_PORTB_AFSEL_R |= 0x10;   // Enable alternate function on PB4
    GPIO_PORTB_DEN_R &= ~0x10;    // Disable digital function on PB4
    GPIO_PORTB_AMSEL_R |= 0x10;   // Enable analog function on PB4


    ADC0_ACTSS_R &= ~0x08;        // Disable SS3 during setup
    ADC0_EMUX_R &= ~0xF000;       // Software trigger
    ADC0_SSMUX3_R = 10;           // Set channel to AIN10 (PB4)
    ADC0_SSCTL3_R = 0x06;         // IE0 and END0 set
    ADC0_ACTSS_R |= 0x08;         // Enable SS3

    ADC0_SAC_R  |= 0x04;          // 16x Hardware averaging
}

uint16_t adc_read(void)
{
    ADC0_PSSI_R = 0x08;
    while((ADC0_RIS_R & 0x08) == 0){}
    int result = ADC0_SSFIFO3_R & 0xFFF;
    ADC0_ISC_R = 0x08;
    return result;
}

int get_distance(uint16_t adc_val)
{
    int dist = 0;
    uint16_t x = adc_val;
    dist = 92625 * pow(x, -1.143);

    return dist;
}
