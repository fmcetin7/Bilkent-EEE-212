#include <MKL25Z4.H>
#include <stdio.h>



//Function Prototypes 
//Either use this or define the functions before main funciton
void Delay(volatile unsigned int time_del);
void LCD_command(unsigned char command);
void LCD_data(unsigned char data);
void LCD_init(void);
void LCD_ready(void);
void keypad_init(void);
void print_fnc(unsigned char *data);
void clear_lcd(void);
uint32_t keypad_getkey(void);
void disco(void);
void delay(uint32_t cycles);
char * print_to_arr(char * buffer, float number, int precision);








void Delay(volatile unsigned int time_del) {
  while (time_del--) 
		{
  }
}


void disco(void){
	
	int counter = 0;
	
	SIM->SCGC5 |= 0x0100;      /* enable clock to Port A*/
	
	PORTA->PCR[1] = PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK; /* make PTA1 pin as GPIO; enable the pull-up resistor */
	PORTA->PCR[2] = PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK; /* make PTA2 pin as GPIO; enable the pull-up resistor */
	PORTA->PCR[5] = PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK; /* make PTA5 pin as GPIO; enable the pull-up resistor */
	
	PORTA->PCR[12] = 0x100;		/* make PTA12 pin as GPIO */
	PORTA->PCR[13] = 0x100;		/* make PTA13 pin as GPIO */
	PORTA->PCR[16] = 0x100;		/* make PTA16 pin as GPIO */
	PORTA->PCR[17] = 0x100;		/* make PTA17 pin as GPIO */
	
	PTA->PDDR &= ~(1UL << 1);    /* make PTA1 as input pin */
	PTA->PDDR &= ~(1UL << 2);    /* make PTA2 as input pin */
	PTA->PDDR &= ~(1UL << 5);    /* make PTA5 as input pin */
	PTA->PDDR |= (1UL << 12);    /* make PTA12 as output pin */
	PTA->PDDR |= (1UL << 13);    /* make PTA13 as output pin */
	PTA->PDDR |= (1UL << 16);    /* make PTA16 as output pin */
	PTA->PDDR |= (1UL << 17);    /* make PTA17 as output pin */
	
	
	//PTA1-swithch2; PTA2-switch3; PTA5-switch1; LEDS: PTA12/PTA13/PTA16/PTA17 
	
	while(1){
		
		PTA->PDOR |= ((1UL | ((counter >> 0) & 0x1)) << 12);
		PTA->PDOR |= ((1UL | ((counter >> 1) & 0x1)) << 13);
		PTA->PDOR |= ((1UL | ((counter >> 2) & 0x1)) << 16);
		PTA->PDOR |= ((1UL | ((counter >> 3) & 0x1)) << 17);
		
		if (((PTA->PDIR & (1UL<<1)) == 1) & ((PTA->PDIR & (1UL<<2)) == 1)) { /*when both switch is open*/
			if ((PTA->PDIR & (1UL<<5)) == 1) { /*when switch1 open; when PTA5 is high*/
				counter = ((counter + 1) % 16);
			}
			else {
				counter = ((counter - 1 + 16) % 16);
			}
			delay(0);
		}
		
		else if (((PTA->PDIR & (1UL<<1)) == 0) & ((PTA->PDIR & (1UL<<2)) == 0)){ /*when both switch is closed*/
			if ((PTA->PDIR & (1UL<<5)) == 1) { /*when switch1 open; when PTA5 is high*/
				counter = ((counter + 1) % 16);
			}
			else {
				counter = ((counter - 1 + 16) % 16);
			}
			delay(250000);
		}
		
		else if (((PTA->PDIR & (1UL<<1)) == 0) & ((PTA->PDIR & (1UL<<2)) == 1)){ /*switch2 is closed; switch3 is open*/
			if ((PTA->PDIR & (1UL<<5)) == 1) { /*when switch1 open; when PTA5 is high*/
				counter = ((counter + 1) % 16);
			}
			else {
				counter = ((counter - 1 + 16) % 16);
			}
			delay(500000);
		}
		
		else if (((PTA->PDIR & (1UL<<1)) == 1) & ((PTA->PDIR & (1UL<<2)) == 0)){ /*switch2 is open; switch3 is closed*/
			if ((PTA->PDIR & (1UL<<5)) == 1) { /*when switch1 open; when PTA5 is high*/
				counter = ((counter + 1) % 16);
			}
			else {
				counter = ((counter - 1 + 16) % 16);
			}
			delay(1000000);
		}
	
	} 

}

