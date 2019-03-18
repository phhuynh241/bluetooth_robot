// DAC.c
// Runs on LM4F120 or TM4C123, 
// edX lab 13 
// Implementation of the 4-bit digital to analog converter
// Daniel Valvano, Jonathan Valvano
// December 29, 2014
// Port B bits 3-0 have the 4-bit DAC

#include "DAC.h"
#include "tm4c123gh6pm.h"

unsigned char wave;

// **************DAC_Init*********************
// Initialize 6-bit DAC 
// Input: none
// Output: none
void DAC_Init(void){unsigned long volatile delay;
//  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOA; // activate port A
//  delay = SYSCTL_RCGC2_R;    // allow time to finish activating
//  GPIO_PORTA_AMSEL_R &= ~0xFC;      // no analog 
//  GPIO_PORTA_PCTL_R &= ~0xFFFFFF00; // regular function
//  GPIO_PORTA_DIR_R |= 0xFC;      // make PA7-2 out
//  GPIO_PORTA_AFSEL_R &= ~0xFC;   // disable alt funct on PA7-2
//  GPIO_PORTA_DEN_R |= 0xFC;      // enable digital I/O on PA7-2
	SYSCTL_RCGC2_R |= 0x01;     // 1) A clock
  delay = SYSCTL_RCGC2_R;           // delay   
  GPIO_PORTA_CR_R = 0xFC;           // 2) allow changes to PA7-2       
  GPIO_PORTA_AMSEL_R = 0x00;        // 3) disable analog function
  GPIO_PORTA_PCTL_R = 0x00000000;   // 4) GPIO clear bit PCTL  
  GPIO_PORTA_DIR_R = 0xFC;          // 5) Output - PA7-2     
  GPIO_PORTA_AFSEL_R = 0x00;        // 6) no alternate function
  GPIO_PORTA_PUR_R = 0x00;          // 7) Disable pull--up resistors bcause output not reading
  GPIO_PORTA_PDR_R = 0x00;          // 8) Disable pulldown resistors bcause output not reading  
  GPIO_PORTA_DEN_R = 0xFC;          // 9) enable digital pins PA7-2
	GPIO_PORTA_DATA_R = 0x00;      // Outputs are low
}


// **************DAC_Out*********************
// output to DAC
// Input: 6-bit data, 0 to 63 
// Output: none
void DAC_Out(unsigned long data){
  GPIO_PORTA_DATA_R = data;
	wave = data;
}
