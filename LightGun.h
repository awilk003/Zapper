/*
 * LEDSwap.c
 *
 * Created: 3/7/2018 11:12:00 AM
 * Author : Alex
 */ 
 #ifndef _LIGHT_GUN
 #define _LIGHT_GUN

#include <avr/io.h>
#include <avr/interrupt.h>

unsigned char trigger = 0x00;
unsigned char detect = 0x00;
unsigned char myled = 0x00;
unsigned short lightD = 0;
unsigned char hit = 0;

enum LG_States { LG_SMStart, LG_s0, LG_s1, LG_s2 } LG_State;

char TickFct_LG(unsigned char curstate, unsigned char* hit, unsigned char* score)
{
	hit = 0;
	LG_State = curstate;
	trigger = ~PINA & 0x01;
	detect =  0x02; // light always detected
		switch(LG_State) 
	{   // Transitions
		case LG_SMStart:  // Initial transition
		LG_State = LG_s0;
		break;
	
		case LG_s0:
		if (trigger && (detect == 2)) 
		{
			LG_State = LG_s1;
		}
		else if(detect == 2)
		{
			LG_State = LG_s2;
		}
		else
		{
			LG_State = LG_s0;
		}
		break;

		case LG_s1:
			LG_State = LG_s0;
		break;

		case LG_s2:
		if(lightD < 500)
		{
			if(trigger)
			{
				LG_State = LG_s1;
			}
			else
			{
				LG_State = LG_s2;
			}
		}
		else
		{
			LG_State = LG_s0;
		}
		break;

		default:
		LG_State = LG_SMStart;
		break;
	} // Transitions

	switch(LG_State) 
	{   // State actions
		case LG_s0:
			lightD = 0;
			led = 0x00;
			break;

		case LG_s1:
			led = 0x80;
			break;

		case LG_s2:
			++lightD;
		break;

		default:
		break;
	} // State actions
	if (led == 0x80)
	{
		hit = 1;
	}
	PORTD = led;
			curstate = LG_State;
			return curstate;
}

#endif

