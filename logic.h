/*
 * LEDSwap.c
 *
 * Created: 3/7/2018 11:12:00 AM
 * Author : Alex
 */ 
 #ifndef _LOGIC
 #define _LOGIC

#include <avr/io.h>
#include <avr/interrupt.h>

//#include <avr/eeprom.h>
 //uint8_t HighScore =  (const eeprom_read_byte((const uint8_t*)0x00));
unsigned char start = 0x00;
unsigned char reset_HS = 0x00;
unsigned char theled = 0x00;
unsigned char gameTime = 150;
unsigned char count = 0;
unsigned char thescore = 0;

enum GL_States { GL_SMStart, GL_s0, GL_s1, GL_s2 } GL_State;

char TickFct_GL(unsigned char curstate, unsigned char* hit, unsigned char* score)
{
	GL_State = curstate;
	if(hit == 1)
	{
		score += 1;
	}
	start = ~PINA & 0x04;
	reset_HS = ~PINA & 0x08;
	switch(GL_State)
	{   // Transitions
		case GL_SMStart:  // Initial transition
		GL_State = GL_s0;
		break;
		
		case GL_s0:
		if (start == 0x04)
		{
			GL_State = GL_s1;
			start = 0x00;
		}
		
		else
		break;

		case GL_s1:
		if (count > gameTime)
		{
			GL_State = GL_s2;
		}

		break;

		case GL_s2:
		if(start == 0x04)
		{
			count = 0;
			score  = 0;
			start = 0x00;
			GL_State = GL_s1;
		}
		else
		break;

		default:
		GL_State = GL_SMStart;
		break;
	} // Transitions

	switch(GL_State)
	{   // State actions
		case GL_s0:
		score = 0;
		count  = 0;
		if(reset_HS == 0x08)
		{
		//	EEPROM_WriteNBytes(0x00,(uint8_t *)0,1);	
		}	
		break;

		case GL_s1:
		count++;
		if(hit)
		{
			++score;
		}
		break;

		case GL_s2:
		break;

		default:
		break;
	} // State actions
	led = score;
	PORTD = led;
	curstate = GL_State;

/*	if(score > HighScore)
	{
		EEPROM_WriteNBytes(0x00,(uint8_t *)&HighScore,1);
	}
	*/
	return curstate;
}


	
	#endif

