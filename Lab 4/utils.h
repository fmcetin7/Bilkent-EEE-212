#include <MKL25Z4.h>

extern volatile char dir_flag = 0;
extern volatile char state_flag = 0;

void Delay(int);
void init_IO(void);
void init_TPM(void);
void init_SysTick(void);
void PORTA_IRQHandler(void);
void SysTick_Handler(void);


void init_IO(){
	SIM->SCGC5 |= 0x0200; /*enable clock to port a*/
	PORTA->PCR[1] |= PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK | PORT_PCR_IRQC(0x0A); /*PTA1 GPIO, pull-up resistor, falling edge interrupt*/
	PTA->PDDR &= ~1UL; /*make PTA1 input*/
	NVIC_ClearPendingIRQ(PORTA_IRQn); /*clear pending interrupts*/
	NVIC_SetPriority(PORTA_IRQn, 128); /*set interrupt priority*/
	NVIC_EnableIRQ(PORTA_IRQn); /*enable IRQ for portA*/
	
}

void init_TPM(){
	SIM->SCGC5 |= 0x0200; /*enable clock to port a*/
	SIM->SCGC6 |= SIM_SCGC6_TPM2_MASK; /*enable clock to TPM2*/
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1); /*set clock source for TPM2 as MCGFLLCLK*/
	PORTA->PCR[2] |= PORT_PCR_MUX(3); /*make PTA2 as TPM2*/
	TPM2->SC |= TPM_SC_CMOD(1) | 7; /*enable the counter and set the prescaler to 128*/
	TPM2->CONTROLS[1].CnSC |= 0x0028; /*set CH1 of TPM2 as output*/
	TPM2->CONTROLS[1].CnV = 163; /*set the compare value*/
	TPM2->MOD = 3277; /*set the modulo value*/

	
}

void init_SysTick(){
	SysTick->CTRL |= 0x0003; /*set clock source, tickint and interrupt enable*/
	SysTick->LOAD = 2499999; /*load value for systick*/
	
}

void PORTA_IRQHandler(){
	NVIC_ClearPendingIRQ(PORTA_IRQn); /*clear pending interrupts for port A*/
	dir_flag++; /*increment direction flag*/
	PORTA->ISFR = ~0UL; /*set 1 of all bits of ISFR*/
}

void SysTick_Handler(){
	if(state_flag < 6)
		state_flag++;
	else 
		state_flag = 0;
	if(dir_flag & 1){
		switch(state_flag){
			case 0:
				TPM2->CONTROLS[1].CnV = 172;
				break;
			case 1:
				TPM2->CONTROLS[1].CnV = 190;
				break;
			case 2:
				TPM2->CONTROLS[1].CnV = 216;
				break;
			case 3:
				TPM2->CONTROLS[1].CnV = 247;
				break;
			case 4:
				TPM2->CONTROLS[1].CnV = 216;
				break;
			case 5:
				TPM2->CONTROLS[1].CnV = 190;
				break;
		}
	} else {
		switch(state_flag){
			case 0:
				TPM2->CONTROLS[1].CnV = 335;
				break;
			case 1:
				TPM2->CONTROLS[1].CnV = 299;
				break;
			case 2:
				TPM2->CONTROLS[1].CnV = 271;
				break;
			case 3:
				TPM2->CONTROLS[1].CnV = 247;
				break;
			case 4:
				TPM2->CONTROLS[1].CnV = 271;
				break;
			case 5:
				TPM2->CONTROLS[1].CnV = 299;
				break;
		}
	}

}

