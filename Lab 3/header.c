
#include <MKL25Z4.H>
#include <stdio.h>
#include <math.h>


void delay(int n) {
 int i;
 for(i = 0; i < n; i++) ;
}


int main(void){
	unsigned long aleyna = 0;
	
	SIM->SCGC5 |= 0x0200;      /* enable clock to Port A*/
	
	
	PORTA->PCR[1] = 0x103; 		/* PTA1-GPIO and pull-up */
	PORTA->PCR[2] = 0x103;		/* PTA2-GPIO and pull-up */
	PORTA->PCR[5] = 0x103;		/* PTA5-GPIO and pull-up */
	PORTA->PCR[12] = 0x100;		/* PTA12-GPIO */
	PORTA->PCR[13] = 0x100;		/* PTA13-GPIO */
	PORTA->PCR[16] = 0x100;		/* PTA16-GPIO */
	PORTA->PCR[17] = 0x100;		/* PTA17-GPIO */
	
	PTA->PDDR &= ~(1UL << 1);    /* make PTA1 input */
	PTA->PDDR &= ~(1UL << 2);    /* make PTA2 input */
	PTA->PDDR &= ~(1UL << 5);    /* make PTA5 input */
	PTA->PDDR |= (1UL << 12);    /* make PTA12 output */
	PTA->PDDR |= (1UL << 13);    /* make PTA13 output */
	PTA->PDDR |= (1UL << 16);    /* make PTA16 output */
	PTA->PDDR |= (1UL << 17);    /* make PTA17 output */
	
	
	//PTA1-switch2; PTA2-switch3; PTA5-switch1; LEDS: PTA12/PTA13/PTA16/PTA17 
	
	while(1){
	
		PTA->PDOR = (PTA->PDOR & ~(1UL << 12)) | (((aleyna >> 3) & 1) << 12);
		PTA->PDOR = (PTA->PDOR & ~(1UL << 13)) | (((aleyna >> 2) & 1) << 13);
		PTA->PDOR = (PTA->PDOR & ~(1UL << 16)) | (((aleyna >> 1) & 1) << 16);
		PTA->PDOR = (PTA->PDOR & ~(1UL << 17)) | (((aleyna >> 0) & 1) << 17);

		if (PTA->PDIR & (1UL<<5)) { /*when switch1 open; when PTA5 is high*/
			if ((PTA->PDIR & (1UL<<1)) & ( (PTA->PDIR & (1UL<<2)) >> 1 )) { /*when both switch open*/
				aleyna = ((aleyna - 1 + 16) % 16);
			}
			
			else if (~(PTA->PDIR | ~(1UL<<1)) & ( (~(PTA->PDIR | ~(1UL<<2))) >> 1 )){ /*when both switch closed*/
				aleyna = ((aleyna - 1 + 16) % 16);
				delay(320000); /*250ms delay*/
			}
			
			else if (~(PTA->PDIR | ~(1UL<<1)) & ( (PTA->PDIR & (1UL<<2)) >> 1 )) { /*switch2 closed; switch3 open*/
				aleyna = ((aleyna - 1 + 16) % 16);
				delay(640000); /*500ms delay*/
			}
			
			else { //if ((PTA->PDIR & (1UL<<1)) & ( (~(PTA->PDIR | ~(1UL<<2))) >> 1 )){/*switch2 open; switch3 closed*/
				aleyna = ((aleyna - 1 + 16) % 16);
				delay(1280000); /*1000ms delay*/
			}
		}
		
		else{ /*when switch1 closed; when PTA5 is low*/
			if ((PTA->PDIR & (1UL<<1)) & ( (PTA->PDIR & (1UL<<2)) >> 1 )) { /*when both switch open*/
				aleyna = ((aleyna + 1) % 16);
			}
			
			else if (~(PTA->PDIR | ~(1UL<<1)) & ( (~(PTA->PDIR | ~(1UL<<2))) >> 1 )){ /*when both switch closed*/
				aleyna = ((aleyna + 1) % 16);
				delay(320000); /*250ms delay*/
			}
			
			else if (~(PTA->PDIR | ~(1UL<<1)) & ( (PTA->PDIR & (1UL<<2)) >> 1 )) { /*switch2 closed; switch3 open*/
				aleyna = ((aleyna + 1) % 16);
				delay(640000);/*500ms delay*/
			}
			
			else { //if ((PTA->PDIR & (1UL<<1)) & ( (~(PTA->PDIR | ~(1UL<<2))) >> 1 )){/*switch2 open; switch3 closed*/
				aleyna = ((aleyna + 1) % 16);
				delay(1280000);/*1000ms delay*/
			}
		}
	}
}


