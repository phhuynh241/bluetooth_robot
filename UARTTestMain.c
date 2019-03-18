

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
  unsigned char i;
  char string[20];  // global to assist in debugging
  unsigned long n;
	unsigned long countr;
	unsigned long countb;
	unsigned long countg;

	PortF_Init();
	SwitchF_Init();
	
	
	
	
  PLL_Init();
  UART_Init();              // initialize UART
	UART2_Init();
  OutCRLF();
	countr = 0; 
	countb = 0; 
	countg = 0; 

UART_OutString("Program reset");
OutCRLF();

  while(1){
	
//====================================================================================
		
    UART_OutString("\n Enter: ");
		OutCRLF();
    UART_InString(string,19);
		//UART1_InString(string, 19);
//=====================================================================================		
		if(string[0] == 'r')
		{		
				countg = countb = 0;   //Reset Count Green  and Blue			
					countr = countr + 1; 
				if( countr%2 == 1){
			   GPIO_PORTF_DATA_R = GPIO_PORTF_DATA_R | 0x02;    // LED is blue / red
				 UART_OutString(" RED  is ON \n");
				 OutCRLF();
				 UART2_OutString("r");
				}

				else if( countr%2 == 0){
			    GPIO_PORTF_DATA_R = GPIO_PORTF_DATA_R & ~0x02; 
				 UART_OutString(" RED  is OFF \n");
				}
													
		}
		else if(string[0] == 'b')
		{
			countg = countr = 0;  // Reset Count Green and Red 
				 	countb = countb + 1; 
				if( countb%2 == 1){
			    GPIO_PORTF_DATA_R = GPIO_PORTF_DATA_R | 0x04;    // LED is  Blue 
				 UART_OutString(" BLUE  is ON=\n");}

				else if( countb%2 == 0){
			    GPIO_PORTF_DATA_R = GPIO_PORTF_DATA_R & ~0x04; 
				 UART_OutString(" BLUE  is OFF=\n");}
													
		}
		else if(string[0] == 'g')
		{
			countb = countr = 0;  // Reset Count Blue and Red 
				  	countg = countg + 1; 
				if( countg%2 == 1){
			    GPIO_PORTF_DATA_R = GPIO_PORTF_DATA_R | 0x08;    // LED is blue / red
				 UART_OutString(" GREEN  is ON=\n");
				 OutCRLF();}

				else if( countg%2 == 0){
			    GPIO_PORTF_DATA_R = GPIO_PORTF_DATA_R & ~0x08; 
				 UART_OutString(" GREEN  is OFF=\n");
				 OutCRLF();}
				
		}
		OutCRLF();

  }
}
