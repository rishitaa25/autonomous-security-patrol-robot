
#include <stdint.h>
#include <inc/tm4c123gh6pm.h>

void adc_init(void);
uint16_t adc_read(void);
int get_distance(uint16_t adc_val);
