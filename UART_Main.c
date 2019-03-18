/****************************************************************************
//Name: Jonathan Rochin, Nolan Mey
//Date: 3/13/17
//Project: Project 2_part1

Description: Part 1 uses UART0(PA0/1) to communicate with computer terminal.
The code checks for r/g/b/sw1/sw0. if input is r, red light is toggled, and 
same goes with the other colors. sw1 turns all all LED's, and sw0 turns off. 
****************************************************************************/


#include "PLL.h"
#include "UART.h"
#include "tm4c123gh6pm.h"
#include "Nokia5110.h"
#include "ADCSWTrigger.h"
#include "SysTick.h"
#include "PWM.h"



void PortF_Init(void);
void SwitchF_Init(void);
void DisableInterrupts(void);
void EnableInterrupts(void);

//stores ADC input value
volatile unsigned long ADCvalue;
volatile unsigned long Duty;
volatile unsigned long High;
volatile unsigned long Low;
volatile unsigned long Counter;
volatile unsigned long pwm_flag;

	
unsigned long In;  // input from PF4

void OutCRLF(void){
  UART_OutChar(CR);
  UART_OutChar(LF);
}
void PortF_Init(void){ volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000020;     // 1) F clock
  delay = SYSCTL_RCGC2_R;           // delay   
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock PortF PF0  
  GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0       
  GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog function
  GPIO_PORTF_PCTL_R = 0x00000000;   // 4) GPIO clear bit PCTL  
  GPIO_PORTF_DIR_R = 0x0E;          // 5) PF4,PF0 input, PF3,PF2,PF1 output   
  GPIO_PORTF_AFSEL_R = 0x00;        // 6) no alternate function
  GPIO_PORTF_PUR_R = 0x11;          // enable pullup resistors on PF4,PF0       
  GPIO_PORTF_DEN_R = 0x1F;          // 7) enable digital pins PF4-PF0        
}
void SwitchF_Init(void){  unsigned long volatile delay;
	
	
  SYSCTL_RCGC2_R |= 0x00000020; // (a) activate clock for port F
  delay = SYSCTL_RCGC2_R;
  GPIO_PORTF_LOCK_R = 0x4C4F434B; // unlock GPIO Port F
  GPIO_PORTF_CR_R = 0x11;         // allow changes to PF4,0
  GPIO_PORTF_DIR_R &= ~0x11;    // (c) make PF4,0 in (built-in button)
  GPIO_PORTF_AFSEL_R &= ~0x11;  //     disable alt funct on PF4,0
  GPIO_PORTF_DEN_R |= 0x11;     //     enable digital I/O on PF4,0
  GPIO_PORTF_PCTL_R &= ~0x000F000F; //  configure PF4,0 as GPIO
  GPIO_PORTF_AMSEL_R &= ~0x11;  //     disable analog functionality on PF4,0
  GPIO_PORTF_PUR_R |= 0x11;     //     enable weak pull-up on PF4,0
  GPIO_PORTF_IS_R &= ~0x11;     // (d) PF4,PF0 is edge-sensitive
  GPIO_PORTF_IBE_R &= ~0x11;    //     PF4,PF0 is not both edges
  GPIO_PORTF_IEV_R &= ~0x11;    //     PF4,PF0 falling edge event
  GPIO_PORTF_ICR_R = 0x11;      // (e) clear flags 4,0
  GPIO_PORTF_IM_R |= 0x11;      // (f) arm interrupt on PF4,PF0
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00400000; // (g) priority 2
  NVIC_EN0_R = 0x40000000;      // (h) enable interrupt 30 in NVIC
}
void GPIOPortF_Handler(void){ // called on touch of either SW1 or SW2
	
  if(GPIO_PORTF_RIS_R&0x01)// SW2 tou
	{
    GPIO_PORTF_ICR_R = 0x01;  // acknowledge flag0
		//increment the mode, and mod with 4
    GPIO_PORTF_DATA_R = 0x0E;    // LED is blue / red
	  UART_OutString(" All LED have benn turned ON");
		OutCRLF();

  }
	 if(GPIO_PORTF_RIS_R&0x10)// SW1 tou
	{
    GPIO_PORTF_ICR_R = 0x10;  // acknowledge flag0
		//increment the mode, and mod with 4
    GPIO_PORTF_DATA_R = 0x00;    // LED is blue / red
	  UART_OutString(" All LED have benn turned OFF");
		OutCRLF();

  }
}

//systick handler
void SysTick_Handler(void)
{

	//read ADC
	ADCvalue = ADC0_InSeq3();
	//display ADC
	Nokia5110_SetCursor(0, 0);
	Nokia5110_OutString("Resistance value:");
	Nokia5110_OutUDec(ADCvalue);

}


//debug code
int main(void){
  unsigned char i;
  char string[20];  
  unsigned long n;

	DisableInterrupts();
	PortF_Init();
	SwitchF_Init();	
  PLL_Init();
	Nokia5110_Init();
	ADC0_InitSWTriggerSeq3_Ch1();         
  UART_Init();             
	UART2_Init();
  OutCRLF();
	SysTick_Init(60000);
	//PWM0B_Init(32000, 16000);
	EnableInterrupts();
	
	

UART_OutString("Program reset");
OutCRLF();

  while(1){
	

		//initiate by checking for input. 
    UART_OutString("\n Enter: ");
		OutCRLF();
    UART_InString(string,19);										//polling
		//UART1_InString(string, 19);
		//ADCvalue = ADC0_InSeq3();
		
		//Nokia displaying ADCvalu
		
		
		
		if(string[0] == 'r')
		{		
				
				if((GPIO_PORTF_DATA_R & 0x02) == 0)
				{
					GPIO_PORTF_DATA_R = GPIO_PORTF_DATA_R | 0x02;    // LED is blue / red
					UART_OutString(" RED  is ON");
					OutCRLF();
					UART2_OutString("r");
					OutCRLF();
					UART2_OutString("ADC value: ");
					UART_OutUDec(ADCvalue);
				}

				else 
				{
					GPIO_PORTF_DATA_R = GPIO_PORTF_DATA_R & ~0x02; 
					UART_OutString(" RED  is OFF");
				}
													
		}
		else if(string[0] == 'b')
		{
				if((GPIO_PORTF_DATA_R & 0x04) == 0)
				{
					GPIO_PORTF_DATA_R = GPIO_PORTF_DATA_R | 0x04;    // LED is  Blue 
					UART_OutString(" BLUE  is ON");
				}

				else
				{
					GPIO_PORTF_DATA_R = GPIO_PORTF_DATA_R & ~0x04; 
					UART_OutString(" BLUE  is OFF");
				}
													
		}
		else if(string[0] == 'g')
		{
				if((GPIO_PORTF_DATA_R & 0x08) == 0)
				{
					GPIO_PORTF_DATA_R = GPIO_PORTF_DATA_R | 0x08;    // LED is blue / red
					UART_OutString(" GREEN  is ON");
					OutCRLF();}

				else
				{
					GPIO_PORTF_DATA_R = GPIO_PORTF_DATA_R & ~0x08; 
					UART_OutString(" GREEN  is OFF");
					OutCRLF();}
				
		}
		OutCRLF();

  }
}
