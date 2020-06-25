/*
 * motorLogic.c
 *
 * Created: 3/7/2018 11:12:00 AM
 * Author : Alex
 */ 
#ifndef _MOTOR
#define _MOTOR

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
// uint8_t HighScore =  eeprom_read_byte(uint8_t*100);

unsigned char current_highscore = 10;
unsigned char motor = 0x00;
unsigned char score  = 0x00;
unsigned char spincount = 0;
unsigned char spun = 0;
unsigned char add = 0x00;


enum M_States { M_SMStart, M_s0, M_s1} M_State;


char TickFct_M(unsigned char curstate, unsigned char* hit, unsigned char* score)
{
	M_State = curstate;


	switch(M_State) 
	{   // Transitions
		case M_SMStart:  // Initial transition
		M_State = M_s0;
		break;
	
		case M_s0:
		if((score > current_highscore) && (spun == 0))
		{
			M_State = M_s1;
			spun = 1;
			break;
		}
		else
			break;

		case M_s1:
		if(spincount < 7)
		{
			break;
		}
		else
			M_State = M_s0;
		break;

		default:
		M_State = M_SMStart;
		break;
	} // Transitions

	switch(M_State) 
	{   // State actions
		case M_s0:
		motor = 0x00;
			break;

		case M_s1:
			motor = 0x01;
			++spincount;
			break;
		default:
		break;
	} // State actions
	curstate = M_State;
	return curstate;
	
}

#endif
