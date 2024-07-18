#include <MKL25Z4.h>
#include "utils.h"
#define SYSCLOCK 8000000
#define TICK_VAL 10
#define TPM_Debug 1


/*  PTA1 -> Button Interrupt Falling Edge Pull-Up Resistor
		PTA2 -> TPM2_CH1
		
*/




int main (void){
	init_TPM();
	init_IO();
	init_SysTick();
	__enable_irq();
	
	while(1){
		__WFI();
	}
}