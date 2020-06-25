/*
 * Zapper.c
 *
 * Created: 3/15/2018 9:34:00 AM
 * Author : Alex
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
// #include "led.h"
//#include "LightGun.h"
//#include "motor.h"
//#include "logic.h"

//#define ~PINA & 0x04;


//#define startbutton ~PINA & 0x04
//#define reset ~PINA & 0x08
//#define trigger ~PINA| 0x01
//led
unsigned char led = 0x00;
unsigned char random_number = 0;
unsigned char radder = 101;
unsigned char bigman = 0;
//LG
unsigned char trigger = 0x00;
unsigned char trigcount = 0;
unsigned char detect = 0x00;
unsigned char myled = 0x00;
unsigned short lightD = 0;
unsigned char hit = 0;
//logic
//uint8_t HighScore =  (const eeprom_read_byte((const uint8_t*)0x00));
unsigned char startbutton =0x00;
unsigned char reset = 0x00;
unsigned char reset_HS = 0x00;
unsigned char theled = 0x00;
unsigned short gameTime = 200;
unsigned char count = 0;
unsigned char thescore = 0;
unsigned char startgame = 0;
//motor
unsigned char current_highscore = 25;
unsigned char motor = 0x00;
unsigned char score  = 0x00;
unsigned char spincount = 0;
unsigned char spun = 0;
unsigned char add = 0x00;

enum LED_States { LED_SMStart, LED_s0, LED_s1 } LED_State;
enum LG_States { LG_SMStart, LG_s0, LG_s1, LG_s2 } LG_State;
enum GL_States { GL_SMStart, GL_s0, GL_s1, GL_s2 } GL_State;
enum M_States { M_SMStart, M_s0, M_s1} M_State;


unsigned char HighestScore = 0;
unsigned char hitt = 0;
unsigned char myScore = 0;
unsigned char eeprom_address=0x00;
unsigned char Start = 0x00;
unsigned char startState = 0;

uint8_t* trial = (uint8_t*) &eeprom_address;

uint8_t myHS;
//myHS = eeprom_read_byte( uint8_t *eeprom_address);
//EEPROM_ReadNBytes(eeprom_address,)(uint8_t *)&HighestScore,1);

volatile unsigned char TimerFlag = 0; // TimerISR() sets this to 1. C programmer should clear to 0

// Internal variables for mapping AVR's ISR to our cleaner TimerISR model.
unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1 ms.
unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1ms ticks

void TimerOn()
{
	// AVR timer/counter controller register TCCR1
	TCCR1B = 0x0B;// bit3 = 0: CTC mode (clear timer on compare)
	// bit2bit1bit0=011: pre-scaler /64
	// 00001011: 0x0B
	// SO, 8 MHz clock or 8,000,000 /64 = 125,000 ticks/s
	// Thus, TCNT1 register will count at 125,000 ticks/s
	// AVR output compare register OCR1A.
	OCR1A = 125000;	// Timer interrupt will be generated when TCNT1==OCR1A
	// We want a 1 ms tick. 0.001 s * 125,000 ticks/s = 125
	// So when TCNT1 register equals 125,
	// 1 ms has passed. Thus, we compare to 125.
	// AVR timer interrupt mask register
	TIMSK1 = 0x02; // bit1: OCIE1A -- enables compare match interrupt

	//Initialize avr counter
	TCNT1=0;

	_avr_timer_cntcurr = _avr_timer_M;
	// TimerISR will be called every _avr_timer_cntcurr milliseconds

	//Enable global interrupts
	SREG |= 0x80; // 0x80: 1000000
}

void TimerOff()
{
	TCCR1B = 0x00; // bit3bit1bit0=000: timer off
}

void TimerISR()
{
	TimerFlag = 1;
}

// In our approach, the C programmer does not touch this ISR, but rather TimerISR()
ISR(TIMER1_COMPA_vect) {
	// CPU automatically calls when TCNT1 == OCR1 (every 1 ms per TimerOn settings)
	_avr_timer_cntcurr--; // Count down to 0 rather than up to TOP
	if (_avr_timer_cntcurr == 0) { // results in a more efficient compare
		TimerISR(); // Call the ISR that the user uses
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

// Set TimerISR() to tick every M ms
void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

typedef struct _task {
	/*Tasks should have members that include: state, period,
		a measurement of elapsed time, and a function pointer.*/
	signed char state; //Task's current state
	unsigned long int period; //Task period
	unsigned long int elapsedTime; //Time elapsed since last task tick
	int (*TickFct)(int); //Task tick function
} task;

unsigned long int findGCD(unsigned long int a, unsigned long int b)
{
	unsigned long int c;
	while(1){
		c = a%b;
		if(c==0){return b;}
		a = b;
		b = c;
	}
	return 0;
}

char TickFct_GL(unsigned char curstate)
{
	trigger = ~PINA & 0x01;
	startbutton = ~PINA & 0x04;
	reset = ~PINA & 0x08;

	GL_State = curstate;
	if(hit == 1)
	{
		//score += 1;
	}
	reset_HS = reset;
	switch(GL_State)
	{   // Transitions
		case GL_SMStart:  // Initial transition
		GL_State = GL_s0;
		break;
		
		case GL_s0:
		if (startbutton == 0x04)
		{
			GL_State = GL_s1;
			startgame = 1;
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
		if(startbutton == 0x04)
		{
			count = 0;
			startgame = 1;
			score = 0;
			spun = 0;
			led = 0;
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
		count  = 0;
		startgame = 0;
		if(reset_HS == 0x08)
		{
			eeprom_update_byte(trial,1);	
		}	
		break;

		case GL_s1:
		count++;
		if(hit)
		{
			score += 1;
		}
		break;

		case GL_s2:
		startgame = 0;
		if(score > current_highscore)
		{
			eeprom_update_byte(trial, score);
		}
		break;

		default:
		break;
	} // State actions
	myled = score;
	PORTD = myled;
	curstate = GL_State;

/*	if(score > HighScore)
	{
		EEPROM_WriteNBytes(0x00,(uint8_t *)&HighScore,1);
	}
	*/
	return curstate;
}

char TickFct_LED(unsigned char curstate)
{
	if(score == 0)
	{
		led = 0x01;
	}
	//trigger = ~PINA & 0x01;
	bigman += radder;
	LED_State = curstate;
	switch(LED_State) {   // Transitions
		case LED_SMStart:  // Initial transition
		LED_State = LED_s0;
		break;
		
		case LED_s0:
		if (!hit) 
		{
			LED_State = LED_s0;
		}
		else if (hit && startgame) 
		{
			LED_State = LED_s1;
		}
		break;

		case LED_s1:
		if (!hit)//trigger) 
		{
			LED_State = LED_s0;
		}
		else if (hit) 
		{
			LED_State = LED_s1;
		}
		break;

		default:
		LED_State = LED_SMStart;
		break;
	} // Transitions

	switch(LED_State) {   // State actions
		case LED_s0:
		//led = 0x01;
		break;

		case LED_s1:
		random_number =  bigman % 2;
		if(random_number == 0)
		{
			led = (led & 0x00) | 0x01;
		}
		else if(random_number == 1)
		{
			led = (led & 0x00) | 0x02;
		}
/*		else if(random_number == 2)
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
		}*/
		break;

		default:
		break;
	} // State actions
	PORTC = led;
	curstate = LED_State;
	return curstate;
}

char TickFct_M(unsigned char curstate)
{
	M_State = curstate;
	myHS = eeprom_read_byte( trial);
	current_highscore = myHS;

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
		if(spincount < 10)
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

	PORTB = motor;

	return curstate;
	
}

char TickFct_LG(unsigned char curstate)
{
	hit = 0;
	LG_State = curstate;
	trigger = ~PINA & 0x01;
	detect =  ~PINA & 0x02; 
	switch(LG_State)
	{   // Transitions
		case LG_SMStart:  // Initial transition
		LG_State = LG_s0;
		break;
		
		case LG_s0:
		if (!trigger && (detect != 2))
		{
			LG_State = LG_s1;
		}
		else if(detect != 2)
		{
			LG_State = LG_s2;
		}
		else
		{
			LG_State = LG_s0;
		}
		break;

		case LG_s1:
		if(trigcount < 10)
		{
			break;
		}
		else
		{
			LG_State = LG_s0;
		}
		LG_State = LG_s0;
		break;

		case LG_s2:
		if(lightD < 500)
		{
			if(!trigger)
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
		theled = 0x00;
		break;

		case LG_s1:
		if(startgame)
		{
			theled = 0x80;
		}
		trigcount += 1;
		break;

		case LG_s2:
		++lightD;
		theled = 0x00;
		break;

		default:
		break;
	} // State actions
	if (theled == 0x80)
	{
		hit = 1;
	}
	PORTD = theled;
	PORTD = PORTD & 0x80;
	curstate = LG_State;
	return curstate;
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
	DDRB = 0xFF; PORTB = 0x00; // Configure port B's 8 pins as outputs,
	DDRC = 0xFF; PORTC = 0x00; // Configure port C's 8 pins as outputs,
	DDRD = 0xFF; PORTD = 0x00; // Configure port D's 8 pins as outputs,
	// while(1) { PORTD = 0xFF; PORTB = 0xFF;}
	// Period for the tasks
	unsigned long int SMTick1_calc = 200;
	unsigned long int SMTick2_calc = 200;
	unsigned long int SMTick3_calc = 200;
	unsigned long int SMTick4_calc = 200;
	 
	//Calculating GCD
	unsigned long int tmpGCD = 200;
	tmpGCD = findGCD(SMTick1_calc, SMTick2_calc);
	tmpGCD = findGCD(tmpGCD, SMTick3_calc);
	tmpGCD = findGCD(tmpGCD, SMTick4_calc);

	//Greatest common divisor for all tasks or smallest time unit for tasks.
	unsigned long int GCD = tmpGCD;

	//Recalculate GCD periods for scheduler
	unsigned long int SMTick1_period = 1000;// SMTick1_calc/GCD;
	unsigned long int SMTick2_period = 1000;//SMTick2_calc/GCD;
	unsigned long int SMTick3_period = 1000;//SMTick3_calc/GCD;
	unsigned long int SMTick4_period = 1000;//SMTick4_calc/GCD;


	static task logic, lightG, ledR, motorT;
	task *tasks[] = { &logic, &lightG, &ledR, &motorT };
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	// Task 1
	logic.state = 1;//Task initial state.
	logic.period = SMTick1_period;//Task Period.
	logic.elapsedTime = SMTick1_period;//Task current elapsed time.
	logic.TickFct = &TickFct_GL;//Function pointer for the tick.

	// Task 2
	lightG.state = 1;//Task initial state.
	lightG.period = SMTick2_period;//Task Period.
	lightG.elapsedTime = SMTick2_period;//Task current elapsed time.
	lightG.TickFct = &TickFct_LG;//Function pointer for the tick.

	// Task 3
	ledR.state = 1;//Task initial state.
	ledR.period = SMTick3_period;//Task Period.
	ledR.elapsedTime = SMTick3_period; // Task current elapsed time.
	ledR.TickFct = &TickFct_LED; // Function pointer for the tick.

	// Task 4
	motorT.state = 1;//Task initial state.
	motorT.period = SMTick4_period;//Task Period.
	motorT.elapsedTime = SMTick4_period; // Task current elapsed time.
	motorT.TickFct = &TickFct_M; // Function pointer for the tick.

	TimerSet(GCD);
	TimerOn();
			volatile unsigned short i = 0;

		while (1) 
		{
		// Scheduler code
		i = 0;
		for (i = 0; i < numTasks; i++ ) {
			// Task is ready to tick
			if ( tasks[i]->elapsedTime == tasks[i]->period ) {
				// Setting next state for task
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);

/*				if(Start != 1)
				{
					if(temp1 == GL_s0)
					{
						--i;
					}
					else if (temp1 == GL_s1)
					{
						Start = 1;
					}
				}
*/				
				// Reset the elapsed time for next tick.
				tasks[i]->elapsedTime = 0;
				}
			tasks[i]->elapsedTime += 1;
		}
		//while(!TimerFlag);
		//TimerFlag = 0;
		}
		return 0;
	}


