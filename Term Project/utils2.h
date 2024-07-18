#include <MKL25Z4.h>
#define RS 0x8 // PTE3 RS
#define RW 0x10 // PTE4 RW
#define EN 0x20 // PTE5 EN

void Delay(volatile unsigned long);
void init(void);
void PORTD_IRQHandler(void);
void game(void);


void LCD_init(void);
void LCD_command(unsigned char command);
void LCD_data(unsigned char data);
void LCD_ready(void);
void clear_lcd(void);
void print_fnc(unsigned char *data);
void main_menu(void);
void game_begin(void);

void keypad_init(void);
uint32_t keypad_getkey(void);
char * print_to_arr(char * buffer, float number, int precision);
int get_number_from_keypad(void);
int wait_for_game_end(void);

void read_to_buffer(int* read_coeff, int len);


volatile int count1, count2, distance1, distance2, goalie;
	
void Delay(volatile unsigned long T){
	while(T--){
	}
}

//PTE29 servo signal; PTE23 ultrasonic trigger 1; PTE22 ultrasonic trigger 2
//PTD0&PTD2 echo inputs
//PTB0,1,2,3,8,9,10,11 data-bus for LCD
//PTE3,4,5 RS,RW,EN
//PTC 0 to 8 keypad inputs

void init(){
	SIM->SCGC5 |= 0x3400; /*clock port B,C,D,E*/

	//servo
	SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK; /*enable tpm0*/
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);  /*set clock source for TPM0 as MCGFLLCLK 41.94MHz*/
	PORTE->PCR[29] |= PORT_PCR_MUX(3); /*TPMO_CH2 PTE29*/
	TPM0->SC |= TPM_SC_CMOD(1) | 7; /*prescaler of TPM0 to 128*/
	TPM0->CONTROLS[2].CnSC |= 0x0028; /*make CH2 output*/
	TPM0->CONTROLS[2].CnV = 246;
	TPM0->MOD = 3279;
	
	// ultrasonic sensor
	SIM->SCGC6 |= SIM_SCGC6_TPM2_MASK; /*enable tpm2*/
	PORTE->PCR[23] = PORT_PCR_MUX(3); /*TPM2_CH1 PTE23*/
	PORTE->PCR[22] = PORT_PCR_MUX(3); /*TPM2_CH0 PTE22*/
	TPM2->SC |= TPM_SC_CMOD(1) | 5; /*set the prescaler to 16*/
	TPM2->CONTROLS[1].CnSC |= 0x0028;
	TPM2->CONTROLS[1].CnV = 13;
	TPM2->CONTROLS[0].CnSC |= 0x0028;
	TPM2->CONTROLS[0].CnV = 13;
	TPM2->MOD = 49971;
	
	PORTD->PCR[0] |= PORT_PCR_MUX(1) | PORT_PCR_IRQC(9) | PORT_PCR_PE_MASK; /*pd resistor; rising-edge interrupt*/
	PORTD->PCR[2] |= PORT_PCR_MUX(1) | PORT_PCR_IRQC(9) | PORT_PCR_PE_MASK; /*pd resistor; rising-edge interrupt*/
	PTD->PDDR &= ~(0x05UL); /*make PTD0&PTD2 input*/
	NVIC_ClearPendingIRQ(PORTD_IRQn);
	NVIC_SetPriority(PORTD_IRQn, 128);
	NVIC_EnableIRQ(PORTD_IRQn);
}


void LCD_init(void)
{
	PORTB->PCR[0] = 0x100;      /* make PTB0 pin as GPIO */
  PORTB->PCR[1] = 0x100;      /* make PTB1 pin as GPIO */
  PORTB->PCR[2] = 0x100;      /* make PTB2 pin as GPIO */
  PORTB->PCR[3] = 0x100;      /* make PTB3 pin as GPIO */
  PORTB->PCR[8] = 0x100;      /* make PTB8 pin as GPIO */
  PORTB->PCR[9] = 0x100;      /* make PTB9 pin as GPIO */
  PORTB->PCR[10] = 0x100;      /* make PTB10 pin as GPIO */
  PORTB->PCR[11] = 0x100;      /* make PTB11 pin as GPIO */
  PTB->PDDR = (0xF << 8) | 0xF;           /* make PTB 11-8 & 3-0 as output pins */
	
  PORTE->PCR[3] = 0x100;      /*RS PTE3 GPIO*/
  PORTE->PCR[4] = 0x100;      /*RW PTE4 GPIO*/
  PORTE->PCR[5] = 0x100;     /*EN PTE5 GPIO*/
  PTE->PDDR |= 0x38;      /*PTE 3,4,5 output*/    
 
	LCD_command(0x38);      /* set 8-bit data, 2-line, 5x7 font */
  LCD_command(0x01);      /* clear screen, move cursor to home */
  LCD_command(0x0F);      /* turn on display, cursor blinking */
	
	main_menu();
}



void keypad_init(void)
{
  SIM->SCGC5 |= 0x0800;       /* enable clock to Port C */ 
  PORTC->PCR[0] = 0x103;      /* make PTC0 pin as GPIO and enable pullup*/
  PORTC->PCR[1] = 0x103;      /* make PTC1 pin as GPIO and enable pullup*/
  PORTC->PCR[2] = 0x103;      /* make PTC2 pin as GPIO and enable pullup*/
  PORTC->PCR[3] = 0x103;      /* make PTC3 pin as GPIO and enable pullup*/
  PORTC->PCR[4] = 0x103;      /* make PTC4 pin as GPIO and enable pullup*/
  PORTC->PCR[5] = 0x103;      /* make PTC5 pin as GPIO and enable pullup*/
  PORTC->PCR[6] = 0x103;      /* make PTC6 pin as GPIO and enable pullup*/
  PORTC->PCR[7] = 0x103;      /* make PTC7 pin as GPIO and enable pullup*/
  PTC->PDDR = 0x00;         /* make PTC7-0 as input pins */
	
}



void game(void)
{
	main_menu();
	get_number_from_keypad();
	if (goalie == 1){
		main_menu();
		print_fnc("ONANA");
	}
	else if (goalie == 2){
		main_menu();
		print_fnc("EDERSON");
	}
	else{
		main_menu();
		print_fnc("COURTOIS");
	}
	Delay(3840000);
//	clear_lcd();
	game_begin();
	__enable_irq();
	wait_for_game_end();
	
	
}

void PORTD_IRQHandler(){
	count2=0;
	count1=0;
	while(PTD->PDIR & 0x05){
		switch(PTD->PDIR & 0x05){
			case 1:
				count2++;
				break;
			case 4:
				count1++;
				break;
			default:
				count1++;
				count2++;
		}
	}

	distance1 = (count1 > 0x400) ? 0x400 : count1 ;
	distance2 = (count2 > 0x400) ? 0x400 : count2 ;
	if (goalie == 1) {
		if ((distance1 < 0x300) & (distance2 < 0x300)) {
			TPM0->CONTROLS[2].CnV = 246; /*0deg*/
		}
		else if (distance1 < 0x300) {
			TPM0->CONTROLS[2].CnV = 192; /*-30deg*/
			Delay(3840000);
		}
		else if (distance2 < 0x300) {
			TPM0->CONTROLS[2].CnV = 301; /*+30deg*/
				Delay(3840000);
		}
		else{
			TPM0->CONTROLS[2].CnV = 246; /*0deg*/
		}
	}
	
	else if (goalie == 2){
		if ((distance1 < 0x300) & (distance2 < 0x300)) {
			TPM0->CONTROLS[2].CnV = 246; /*0deg*/
		}
		else if (distance1 < 0x300) {
			TPM0->CONTROLS[2].CnV = 137; /*-60deg*/
			Delay(3840000);
		}
		else if (distance2 < 0x300) {
			TPM0->CONTROLS[2].CnV = 355; /*+60deg*/
			Delay(3840000);
		}
		else{
			TPM0->CONTROLS[2].CnV = 246; /*0deg*/
		}
	}
	else { //if (goalie == 3)
		if ((distance1 < 0x300) & (distance2 < 0x300)) {
			TPM0->CONTROLS[2].CnV = 246; /*0deg*/
		}
		else if (distance1 < 0x300) {
			TPM0->CONTROLS[2].CnV = 82; /*-90deg*/
			Delay(3840000);
		}
		else if (distance2 < 0x300) {
			TPM0->CONTROLS[2].CnV = 410; /*+90deg*/
			Delay(3840000);
		}
		else{
			TPM0->CONTROLS[2].CnV = 246; /*0deg*/
		}
	}
	NVIC_ClearPendingIRQ(PORTD_IRQn);
	PORTD->ISFR = ~0UL;
	
}







/* This function waits until LCD controller is ready to
 * accept a new command/data before returns.
 */
void LCD_ready(void)
{
    uint32_t status;
    
    PTB->PDDR &= ~((0xF << 8) | 0xF);          /* PTC17-10 input */
    PTE->PCOR = RS;         /* RS = 0 for status */
    PTE->PSOR = RW;         /* R/W = 1, LCD output */
    
    do {    /* stay in the loop until it is not busy */
			  PTE->PCOR = EN;
			  Delay(500);
        PTE->PSOR = EN;     /* raise E */
        Delay(500);
        status = PTB->PDIR & ((0xF << 8) | 0xF); /* read status register */
        PTE->PCOR = EN;
        Delay(500);			/* clear E */
    } while (status & 0x800UL);    /* check busy bit */
    
    PTE->PCOR = RW;         /* R/W = 0, LCD input */
    PTB->PDDR |= (0xF << 8) | 0xF;       /* PTC17-10 output */
}

void LCD_command(unsigned char command)
{
    LCD_ready();			/* wait until LCD is ready */
    PTE->PCOR = RS | RW;    /* RS = 0, R/W = 0 */
    PTB->PDOR = ((command & 0xF0) << 4) | (command & 0xF);
    PTE->PSOR = EN;         /* pulse E */
    Delay(500);
    PTE->PCOR = EN;
}

void LCD_data(unsigned char data)
{
    LCD_ready();			/* wait until LCD is ready */
    PTE->PSOR = RS;         /* RS = 1, R/W = 0 */
    PTE->PCOR = RW;
    PTB->PDOR = ((data & 0xF0) << 4) | (data & 0xF);
    PTE->PSOR = EN;         /* pulse E */
    Delay(500);
    PTE->PCOR = EN;
}

void clear_lcd(void)
{	
	int i;
	LCD_command(0x80); //Start from the 1st line
	for(i = 16; i > 0; i--)
	{
			LCD_data(' '); //Clear the 1st line
	}
	LCD_command(0xC0); //Go to the 2nd line
	for(i = 16; i > 0; i--)
	{
			LCD_data(' '); //Clear the 2nd line
	}
	LCD_command(0x80);
}

void print_fnc(unsigned char *data)
{
	int i = 0 ;
	//Continue until a NULL char comes
	while(data[i] != 0x00)
	{
		LCD_data(data[i]);
		i++;
	}
}

void main_menu(void)
{
	clear_lcd();
	LCD_command(0x80);
	print_fnc("Select Keeper:");
	LCD_command(0xC0);
}

void game_begin(void)
{
	clear_lcd();
	LCD_command(0x80);
	print_fnc("Take Your Shot!");
	LCD_command(0xC0);
	if (goalie == 1){
		print_fnc("ONANA");
	}
	else if (goalie == 2){
		print_fnc("EDERSON");
	}
	else{
		print_fnc("COURTOIS");
	}
}



int get_number_from_keypad(){
		uint32_t key;
		char lookup[]= {'1','2','3','A','4','5','6','B','7','8','9','C','*','0','#','D'};
		key = 0;
		while(lookup[key-1] != '#'){
			Delay(300000);	
			key=keypad_getkey();
				if((key != 0)&(lookup[key-1] != '#')){
					LCD_data(lookup[key-1]);
					goalie = key;
					Delay(300000);
					key =0;
				}
		}
		return key;
}


uint32_t keypad_getkey(void)
{
    uint32_t row, col;
    const char row_select[] = {0x01, 0x02, 0x04, 0x08}; /* one row is active */

    /* check to see any key pressed */
    PTC->PDDR |= 0x0F;          /* rows output */
    PTC->PCOR = 0x0F;               /* ground rows */
    Delay(500);                 /* wait for signal return */
    col =  PTC->PDIR & 0xF0UL;     /* read all columns */
    PTC->PDDR = 0;              /*  rows input */
    if (col == 0xF0UL)
        return 0;               /* no key pressed */

    /* If a key is pressed, it gets here to find out which key.
     * It activates one row at a time and read the input to see
     * which column is active. */
    for (row = 0; row < 4; row++)
    {
        PTC->PDDR = 0;                  /* disable all rows */
        PTC->PDDR |= row_select[row];   /* enable one row */
        PTC->PCOR = row_select[row];    /* drive the active row low */
        Delay(500);                     /* wait for signal to settle */
        col = PTC->PDIR & 0xF0UL;         /* read all columns */
        if (col != 0xF0UL) break;         /* if one of the input is low, some key is pressed. */
    }
    PTC->PDDR = 0;                      /* disable all rows */
    if (row == 4) 
        return 0;                       /* if we get here, no key is pressed */
 
    /* gets here when one of the rows has key pressed, check which column it is */
    if (col == 0xE0UL) return row * 4 + 1;    /* key in column 0 */
    if (col == 0xD0UL) return row * 4 + 2;    /* key in column 1 */
    if (col == 0xB0UL) return row * 4 + 3;    /* key in column 2 */
    if (col == 0x70UL) return row * 4 + 4;    /* key in column 3 */

    return 0;   /* just to be safe */
}

int wait_for_game_end(){
		uint32_t key;
		char lookup[]= {'1','2','3','A','4','5','6','B','7','8','9','C','*','0','#','D'};
		key = 0;
		while(lookup[key-1] != '*'){
			Delay(300000);	
			key=keypad_getkey();
				if((key != 0)&(lookup[key-1] != '*')){
					Delay(300000);
					key =0;
				}
		}
		return key;
}


//void read_to_buffer(int* read_coeff, int len){
//	char read_buffer[]= {'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'};
//	int key;
//	char lookup[]= {'1','2','3','A','4','5','6','B','7','8','9','C','*','0','#','D'};
//	int key_prev = 0;
//	key = 1;
//	int idx;
//	idx = 0;
//	
//	while(keypad_getkey() == 0)//Polling, waiting to get an input
//	{
//	}
//	while( (lookup[key-1] != '#')||(idx < len+1) ) // Take input until the # key is pressed or screen limit exceeded
//	{
//		key = 0;
//		Delay(300000);
//		while(key == 0)//Polling, waiting to get an input, don't read the same key every time.
//		{
//			key=keypad_getkey();
//			if(key_prev == key) // Prevents reading the same key.
//			{
//				key = 0;
//			}
//			else
//			{
//				key_prev = key;
//			}
//		}
//		if(key != 0)// There is an input
//		{
//			read_buffer[idx] = lookup[key-1];
//			read_coeff[idx] = lookup[key-1]-'0';
//			idx = idx + 1;
//			clear_lcd();
//			print_fnc(read_buffer);
//		}
//	}
//	read_coeff[idx-1] = 0;
//}

//char * print_to_arr(char * buffer, float number, int precision){
//	
//	int significant = (int) number;
//	int s = (int) number;
//	number = number - (float) significant;
//	for(int i =0; i < precision; i++){
//			number = number*10;
//	}
//	int point = (int) number;
//	
//	int idx = 0;
//	int q = 1;
//	while(s > 0){
//		s = s /10 ;
//		idx = idx + 1;
//		q = q*10;
//	}
//	for(int i = 0 ; i < idx ; i++){
//		q = q/10;
//		buffer[i] = (significant/q) + '0';
//		significant =  significant-(significant/q)*q;
//	}
//	
//	buffer[idx] = '.';
//	for(int i = idx + precision ; idx < i ; i--){
//		buffer[i] = (point - (point/10)*10) + '0';
//		point = point /10 ;
//	}
//	return buffer;
//}





