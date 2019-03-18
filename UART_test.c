// UARTTestMain.c
// Runs on LM4F120/TM4C123
// Used to test the UART.c driver
// Daniel Valvano
// September 12, 2013

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2013

 Copyright 2013 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// U0Rx (VCP receive) connected to PA0
// U0Tx (VCP transmit) connected to PA1

#include "PLL.h"
#include "UART.h"
#include "tm4c123gh6pm.h"

//---------------------OutCRLF---------------------
// Output a CR,LF to UART to go to a new line
// Input: none
// Output: none
void PortF_Init(void);
void SwitchF_Init(void);
	
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

  }
	 if(GPIO_PORTF_RIS_R&0x10)// SW1 tou
	{
    GPIO_PORTF_ICR_R = 0x10;  // acknowledge flag0
		//increment the mode, and mod with 4
    GPIO_PORTF_DATA_R = 0x00;    // LED is blue / red
	  UART_OutString(" All LED have benn turned OFF");

  }
}
//debug code
int main(void){
  unsigned int i;
  char string[20];  // global to assist in debugging
  unsigned long n;
	unsigned long countr;
	unsigned long countb;
	unsigned long countg;

	PortF_Init();
	SwitchF_Init();
	
	
	
	
  PLL_Init();
  UART_Init();              // initialize UART
	UART_Init2();
	UART2_Init();
  OutCRLF();
	countr = 0; 
	countb = 0; 
	countg = 0; 

  

  while(1){
	
//====================================================================================
		
    //UART_OutString("\n Enter: ");
    //UART_InString(string,19);
		//UART1_InString(string, 19);
//=====================================================================================	
		UART2_InString(string, 19);		
		UART1_OutString("r");	//output UART1 to UART2
		if(string[0] == 'r')
		{
			GPIO_PORTF_DATA_R = 0x02;
		}
		else
		{
			GPIO_PORTF_DATA_R = 0x04;
		}
		for(i = 0; i < 50000; i = i + 1) {}
		
  }
}
