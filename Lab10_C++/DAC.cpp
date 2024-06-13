// put implementations for functions, explain how it works
// put your names here, date
// dac.cpp
// This software configures DAC output
// Runs on TM4C123
// Program written by: Aaron Phan, Jin Jun Oh
// Date Created: 4/16/23 
// Last Modified: 4/16/23 
// Lab number: 6
// Hardware connections
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "Images.h"
#include "DAC.h"
#include "ST7735.h"
#include "Sound.h"

volatile uint32_t flag2;
volatile uint32_t flag3;
extern uint8_t shot_count;
extern uint8_t level;
extern uint8_t points;
extern volatile uint8_t button1; extern volatile uint8_t button2;
uint8_t pause_flag = 0;
// **************DAC_Init*********************
// Initialize 6-bit DAC, called once 
// Input: none
// Output: none
void DAC_Init(void){
  // used in Lab 6 and lab 10
	uint32_t volatile delay;
  SYSCTL_RCGCGPIO_R = SYSCTL_RCGCGPIO_R |= 0x02; // activate port B
  delay = SYSCTL_RCGCGPIO_R; // allow time to finish PortB clock activating
  GPIO_PORTB_DIR_R |= 0x3F;  // make PB5-0 out
  GPIO_PORTB_DEN_R |= 0x3F;  // enable digital I/O on PB5-0
	
}
// **************DAC_Out*********************
// output to DAC
// Input: 6-bit data, 0 to 63 
// Input=n is converted to n*3.3V/63
// Output: none
void DAC_Out(uint32_t data){
  // used in Lab 6 and lab 10
	GPIO_PORTB_DATA_R = data;
	
}
//Switch Methods and Initializations

void Switch_Init(void){  
	uint32_t volatile delay;
	SYSCTL_RCGCGPIO_R |= 0x10;        // 1) activate port E
  while((SYSCTL_PRGPIO_R&0x10)==0){};   // allow time for clock to stabilize
                                    // 2) no need to unlock PE3-0
  GPIO_PORTE_AMSEL_R &= ~0x0F;      // 3) disable analog functionality on PE3-0
  GPIO_PORTE_PCTL_R &= ~0x0000FFFF; // 4) GPIO
  GPIO_PORTE_DIR_R &= ~0x0F;         // 5) make PE3-0 input
  GPIO_PORTE_AFSEL_R &= ~0x0F;      // 6) regular port function
  GPIO_PORTE_DEN_R |= 0x0F;         // 7) enable digital I/O on PE3-0
	flag3 = 0;
	flag2 = 0;
	
}
void Button_Press(int a, int b){
			int aa = a; int bb = b;
			
			if(button1 == 1 && flag2 == 0){ // if PE0 is pressed
				Sound_Shoot();
				flag2 = 1;
				Collision(aa,bb);
			}	
			else if(button1 == 0 && flag2 == 1){ // if PE0 is not pressed
				flag2 = 0;
				
			}	
			else if(button2 ==1 && flag3 == 0){ // if PE1 is pressed
				Sound_Shoot();
				flag3 = 1;
				pause_flag = 1;
				while(button2 == 1){};
				Pause_Screen();
				
			}	
			else if(button2 == 0 && flag3 == 1){ // if PE1 is not pressed
				flag3 =0;
			}	
}
//a is duck, b is user
void Collision(int a, int b){
		
	if((a-30)<b && b<(a+30)){
		Sound_duck();
		points++;
		ST7735_SetCursor(8,2);
		ST7735_OutUDec(points);
		shot_count = 3;
		ST7735_SetCursor(7,0);
		ST7735_OutUDec(shot_count);
	}
	else { 
		shot_count--;
		ST7735_SetCursor(7,0);
		ST7735_OutUDec(shot_count);
	}
}
void Game_Over(void){
	TIMER1_CTL_R = 0x00000000;    // 1) disable TIMER1A during setup
	ST7735_FillScreen(0);
	while(1){
	ST7735_DrawBitmap(0,160,loss_screen,128,160);
	} 
}
void Pause_Screen(void){
	TIMER1_CTL_R = 0x00000000;    // 1) disable TIMER1A during setup
	while(pause_flag == 1){
	ST7735_DrawBitmap(0,160,pause,128,160);
	if(button2 == 1) pause_flag = 0;
	}
	TIMER1_CTL_R = 0x00000001;    // 10) enable TIMER1A
	ST7735_FillScreen(ST7735_CYAN);
	ST7735_SetCursor(0, 0);
	ST7735_OutString((char*)"Shots: ");
	ST7735_OutUDec(shot_count);
	ST7735_SetCursor(0, 1);
	ST7735_OutString((char*)"Level: ");
	ST7735_OutUDec(level);
	ST7735_SetCursor(0, 2);
	ST7735_OutString((char*)"Points: ");
	ST7735_OutUDec(points);
}
