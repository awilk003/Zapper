/*
 * LEDSwap.c
 *
 * Created: 3/7/2018 11:12:00 AM
 * Author : Alex
 */ 
 #ifndef _LED
 #define _LED

#include <avr/io.h>
#include <avr/interrupt.h>

unsigned char button = 0x00;
unsigned char led = 0x00;
unsigned char random_number = 0;
unsigned char radder = 101;
unsigned char bigman = 0;

enum LED_States { LED_SMStart, LED_s0, LED_s1 } LED_State;



char TickFct_LED(unsigned char curstate, unsigned char* hit, unsigned char* score)
{
	bigman += radder;
	LED_State = curstate;
	button = ~PINA & 0x01;
	switch(LED_State) {   // Transitions
		case LED_SMStart:  // Initial transition
		LED_State = LED_s0;
		break;
	
		case LED_s0:
		if (!button) {
			LED_State = LED_s0;
		}
		else if (button) {
			LED_State = LED_s1;
		}
		break;

		case LED_s1:
		if (!button) {
			LED_State = LED_s0;
		}
		else if (button) {
			LED_State = LED_s1;
		}
		break;

		default:
		LED_State = LED_SMStart;
		break;
	} // Transitions

	switch(LED_State) {   // State actions
		case LED_s0:
		break;

		case LED_s1:
			random_number =  bigman % 8;
			 if(random_number == 0)
			{
				led = (led & 0x00) | 0x01;
			}
			else if(random_number == 1)
			{
				led = (led & 0x00) | 0x02;
			}
			else if(random_number == 2)
			{  
				led = (led & 0x00) | 0x04;
			}
			else if(random_number == 3)
			{
				led = (led & 0x00) | 0x08;
			}
			else if(random_number == 4)
			{
				led = (led & 0x00) | 0x10;
			}
			else if(random_number == 5)
			{
				led = (led & 0x00) | 0x20;
			}
			else if(random_number == 6)
			{
				led = (led & 0x00) | 0x40;
			}
			else if(random_number == 7)
			{
				led = (led & 0x00) | 0x80;
			}
			break;

		default:
		break;
	} // State actions
	PORTC = led;
	curstate = LED_State;
	return curstate;
}
#endif


