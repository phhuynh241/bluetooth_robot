#include "PLL.h"
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "UART.h"
#include "SysTick.h"
#include "Sound.h"
#include "DAC.h"
#include "Sound.h"

void PortF_Init(void);
void SwitchF_Init(void);
void DisableInterrupts(void);
void EnableInterrupts(void);
void PWM1F_Duty(unsigned int duty);

unsigned int mode = 0;
unsigned long Index = 0;

#define OFF_LED 0xF1
#define GREEN_LED 0x08
#define RED_LED 0x02
#define BLUE_LED 0x04
#define WHITE_LED 0x0E
#define YELLOW_LED 0x0A
#define PURPLE_LED 0x06
#define CYAN_LED 0x0C

#define _BHz 3170 //B
#define _AHz 3551 //A
#define _GHz 3986 //G
#define _FHz 4477 //F
#define _EHz 4749 //E
#define _DHz 5333 //D
#define _CHz 5987 //C

const unsigned char SineWave[64] = {0x7e,0x8a,0x97,0xa3,0xae,0xb9,0xc4,0xce,
0xd7,0xdf,0xe7,0xed,0xf2,0xf7,0xfa,0xfb,
0xfc,0xfb,0xfa,0xf7,0xf2,0xed,0xe7,0xdf,
0xd7,0xce,0xc4,0xb9,0xae,0xa3,0x97,0x8a,
0x7e,0x72,0x65,0x59,0x4e,0x43,0x38,0x2e,
0x25,0x1d,0x15,0xf,0xa,0x5,0x2,0x1,
0x0,0x1,0x2,0x5,0xa,0xf,0x15,0x1d,
0x25,0x2e,0x38,0x43,0x4e,0x59,0x65,0x72};

const unsigned char TriWave[64] = {0x8,0x10,0x18,0x20,0x27,0x2f,0x37,0x3f,
0x47,0x4f,0x57,0x5f,0x66,0x6e,0x76,0x7e,
0x86,0x8e,0x96,0x9e,0xa5,0xad,0xb5,0xbd,
0xc5,0xcd,0xd5,0xdd,0xe4,0xec,0xf4,0xfc,
0xf4,0xec,0xe4,0xdd,0xd5,0xcd,0xc5,0xbd,
0xb5,0xad,0xa5,0x9e,0x96,0x8e,0x86,0x7e,
0x76,0x6e,0x66,0x5f,0x57,0x4f,0x47,0x3f,
0x37,0x2f,0x27,0x20,0x18,0x10,0x8,0x00};

const unsigned char SquWave[64] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,
0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,
0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,
0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC
};

void Delay10ms(void){unsigned long volatile time;
  time = 14545;  // 10msec
  while(time){
		time--;
  }
}

void Delay100ms(void){unsigned long volatile time;
	time = 2500000; //10,000,000
	while(time){
		time--;
	}
}

void Delay1000ms(void){unsigned long volatile time;
	time = 5000000;
	while(time){
		time--;
	}
}

void Motor_Init(void){
	unsigned volatile long delay;
  SYSCTL_RCGC2_R |= 0x00000004; // activate clock for port C
	delay = SYSCTL_RCGC2_R;
  GPIO_PORTC_AMSEL_R &= ~0xF0;      // disable analog functionality on PC7-4
  GPIO_PORTC_PCTL_R &= ~0xFFFF0000; // configure PC7-4 as GPIO
  GPIO_PORTC_DIR_R |= 0xF0;     // make PC7-4 out
  GPIO_PORTC_DR8R_R |= 0xF0;    // enable 8 mA drive on PC7-4
  GPIO_PORTC_AFSEL_R &= ~0xF0;  // disable alt funct on PC7-4
  GPIO_PORTC_DEN_R |= 0xF0;     // enable digital I/O on PC7-4
  GPIO_PORTC_DATA_R &= ~0xF0;   // make PC7-4 low
}

void SwitchF_Init(void){  unsigned long volatile delay;
  SYSCTL_RCGC2_R |= 0x00000020; // (a) activate clock for port F
  delay = SYSCTL_RCGC2_R;
  GPIO_PORTF_LOCK_R = 0x4C4F434B; // unlock GPIO Port F
  GPIO_PORTF_CR_R = 0x1F;         // allow changes to PF4,0
  GPIO_PORTF_DIR_R &= ~0x11;    // (c) make PF4,0 in (built-in button)
	GPIO_PORTF_DIR_R |= 0x0E;
  GPIO_PORTF_AFSEL_R &= ~0x1F;  //     disable alt funct on PF4-0
  GPIO_PORTF_DEN_R |= 0x1F;     //     enable digital I/O on PF4-0
  GPIO_PORTF_PCTL_R &= ~0x000FFFFF; //  configure PF4-0 as GPIO
  GPIO_PORTF_AMSEL_R &= ~0x1F;  //     disable analog functionality on PF4-0
  GPIO_PORTF_PUR_R |= 0x1F;     //     enable weak pull-up on PF4-0
 
	GPIO_PORTF_IS_R &= ~0x11;     // (d) PF4,PF0 is edge-sensitive
  GPIO_PORTF_IBE_R &= ~0x11;    //     PF4,PF0 is not both edges
  GPIO_PORTF_IEV_R &= ~0x11;    //     PF4,PF0 falling edge event
  GPIO_PORTF_ICR_R = 0x11;      // (e) clear flags 4,0
  GPIO_PORTF_IM_R |= 0x11;      // (f) arm interrupt on PF4,PF0
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00400000; // (g) priority 2
  NVIC_EN0_R = 0x40000000;      // (h) enable interrupt 30 in NVIC
}

int main(void){
	unsigned char CMD;
	int k;
	DisableInterrupts();
	Motor_Init();	//Turns on the motor
	SwitchF_Init();	//
	PLL_Init();
	DAC_Init();
	UART_Init();  //UART0
	UART_Init_1(); //UART1
	Sound_Tone(0);
	EnableInterrupts();
	GPIO_PORTF_DATA_R = CYAN_LED; //test to see if it goes thru setup
  while(1){
		CMD = UART1_InChar();
		if(CMD == 'w'){
			GPIO_PORTC_DATA_R = 0x00;
			for (k = 0; k < 100; k++){};
			GPIO_PORTC_DATA_R = 0xC0;   // 0011 0000 turn on Pin 4 5 on Hbridge to go forward
			GPIO_PORTF_DATA_R = GREEN_LED;		//red LED
		}
		else if(CMD == 's'){
			GPIO_PORTC_DATA_R = 0x00;
			for (k = 0; k < 100; k++){};
			GPIO_PORTC_DATA_R = 0x30;   // 1100 0000 turn on pin 6 7 go back ward 
			GPIO_PORTF_DATA_R = BLUE_LED;	//Blue LED
		}	
		else if(CMD == 'a'){
			GPIO_PORTC_DATA_R = 0x00;
			for (k = 0; k < 100; k++){};
			GPIO_PORTC_DATA_R = 0x90;   //1001 0000 turn on 9 and 4 turn left
			GPIO_PORTF_DATA_R = PURPLE_LED;	//Purple LED
		}
		else if(CMD == 'd'){
			GPIO_PORTC_DATA_R = 0x00;
			GPIO_PORTC_DATA_R = 0x60;   // 0110 0000 turn right 
			GPIO_PORTF_DATA_R = YELLOW_LED;		
		}		
		else if (CMD == 'q'){	
			GPIO_PORTC_DATA_R = 0x00;		// Stops motor
			GPIO_PORTF_DATA_R = 0x02; 	//Red LED
		}
		
		else if (CMD == 'm'){
			GPIO_PORTF_DATA_R = WHITE_LED; //GREEN LED
			GPIO_PORTC_DATA_R = 0x00;
			
			//Twinkle Twinkle Little Star
			Sound_Tone(_CHz);
			Delay100ms();
			Sound_Tone(_CHz);
			Delay100ms();
			Sound_Tone(_GHz);
			Delay100ms();
			Sound_Tone(_GHz);
			Delay100ms();
			Sound_Tone(_AHz);
			Delay100ms();
			Sound_Tone(_AHz);
			Delay100ms();
			Sound_Tone(_GHz);
			Delay1000ms();
			
			Sound_Tone(_FHz);
			Delay100ms();
			Sound_Tone(_FHz);
			Delay100ms();
			Sound_Tone(_EHz);
			Delay100ms();
			Sound_Tone(_EHz);
			Delay100ms();
			Sound_Tone(_DHz);
			Delay100ms();
			Sound_Tone(_DHz);
			Delay100ms();
			Sound_Tone(_CHz);
			Delay1000ms();
			
			Sound_Tone(_GHz);
			Delay100ms();
			Sound_Tone(_GHz);
			Delay100ms();
			Sound_Tone(_FHz);
			Delay100ms();
			Sound_Tone(_FHz);
			Delay100ms();
			Sound_Tone(_EHz);
			Delay100ms();
			Sound_Tone(_EHz);
			Delay100ms();
			Sound_Tone(_DHz);
			Delay1000ms();
			Sound_Tone(0);
		}
  }
}

void SysTick_Handler(void){
	Index = (Index+1)&0x3F;
	DAC_Out(SineWave[Index]); //outputs Sine Wave
}
