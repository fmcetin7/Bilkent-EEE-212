#include <MKL25Z4.h>
#include "utils2.h"

//PTE29 servo signal; PTE23 ultrasonic trigger 1; PTE22 ultrasonic trigger 2
//PTD0&PTD2 echo inputs
//PTB0,1,2,3,8,9,10,11 data-bus for LCD
//PTE3,4,5 RS,RW,EN
//PTC 0 to 8 keypad inputs



int main(){
	__disable_irq();
	init();
	LCD_init();
	keypad_init();

//	game();
//	clear_lcd();
//	__enable_irq();
	
	while(1){
//		get_number_from_keypad();
//		if (game_on == 0)
//		{
			game();
//		}
//		wait_for_game_end();
//		__WFI();
	}
}
