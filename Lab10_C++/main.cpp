// main.cpp
// Runs on LM4F120/TM4C123
// Jonathan Valvano and Daniel Valvano
// This is a starter project for the ECE319K Lab 10 in C++

// Last Modified: 1/2/2023 
// http://www.spaceinvaders.de/
// sounds at http://www.classicgaming.cc/classics/spaceinvaders/sounds.php
// http://www.classicgaming.cc/classics/spaceinvaders/playguide.php

// ******* Possible Hardware I/O connections*******************
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PD2/AIN5
// Slide pot pin 3 connected to +3.3V 
// fire button connected to PE0
// special weapon fire button connected to PE1
// 32*R resistor DAC bit 0 on PB0 (least significant bit)
// 16*R resistor DAC bit 1 on PB1 
// 8*R resistor DAC bit 2 on PB2
// 4*R resistor DAC bit 1 on PB3
// 2*R resistor DAC bit 2 on PB4
// 1*R resistor DAC bit 3 on PB5 (most significant bit)
// LED on PB6
// LED on PB7

// Backlight (pin 10) connected to +3.3 V
// MISO (pin 9) unconnected
// SCK (pin 8) connected to PA2 (SSI0Clk)
// MOSI (pin 7) connected to PA5 (SSI0Tx)
// TFT_CS (pin 6) connected to PA3 (SSI0Fss)
// CARD_CS (pin 5) unconnected
// Data/Command (pin 4) connected to PA6 (GPIO), high for data, low for command
// RESET (pin 3) connected to PA7 (GPIO)
// VCC (pin 2) connected to +3.3 V
// Gnd (pin 1) connected to ground

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "PLL.h"
#include "ST7735.h"
#include "random.h"
#include "PLL.h"
#include "SlidePot.h"
#include "Images.h"
#include "UART.h"
#include "Timer0.h"
#include "Timer1.h"
#include "Sound.h"
#include "DAC.h"

SlidePot my(2000,0);

extern "C" void DisableInterrupts(void);
extern "C" void EnableInterrupts(void);
extern "C" void SysTick_Handler(void);

uint32_t language; // 0 means english 1 means spanish
uint32_t period = 100000000;
volatile uint32_t flag;
uint8_t shot_count = 3;
uint8_t level = 1;
volatile uint8_t button1; volatile uint8_t button2;
extern uint8_t pause_flag;
uint8_t points = 0;
uint32_t counter = 0;
typedef enum {dead,alive} status_t;

struct sprite{
  uint32_t x;      // x coordinate
  uint32_t y;      // y coordinate
  const unsigned short *image; // ptr->image
  status_t life;            // dead/alive
};          
typedef struct sprite sprite_t;

void SysTick_Init(void){
	NVIC_ST_CTRL_R = 0;              // 1) disable SysTick during setup
  NVIC_ST_RELOAD_R = 2666666;   // 2) maximum reload value
  NVIC_ST_CURRENT_R = 0;           // 3) any write to current clears it
  NVIC_ST_CTRL_R = 0x00000007;     // 4) enable SysTick with core clock
}

void SysTick_Handler(void){
		flag = 1;
		counter++;
		uint32_t sample = ADC_In();
		my.Save(sample);
				button1 = GPIO_PORTE_DATA_R & 0x01;
				button2 = (GPIO_PORTE_DATA_R & 0x02) >> 1;
				
}



// Creating a struct for the Sprite.

	sprite_t bill={0,0,gun,alive};
	sprite_t duck={96,9,duck_pic,alive};
	uint32_t time = 0;
	
void background(void){
  flag = 1; // semaphore
  if(bill.life == alive){
    bill.y++;
  }
  if(bill.y>155){
    bill.life = dead;
  }
}
void clock(void){
		time++;
		//duck movement
				
		int32_t duck_pre = duck.y;
		duck.y = (Random()>>1) + 30; if(duck.y < 70) duck.y = 70;	//max value of 128 from Random(), subtract if over max x range of user
		if (duck_pre > duck.y){		
			ST7735_FillRect(96, duck.y , 40, 140, ST7735_CYAN);
		}
		else if (duck_pre < duck.y ){		
			ST7735_FillRect(96, duck.y - 120, 40, 140, ST7735_CYAN);
		}
		ST7735_DrawBitmap(duck.x,duck.y,duck.image,32,36);

}
void ducksound(void){
		
}
	
int main(void){
	DisableInterrupts();
  PLL_Init(Bus80MHz);       // Bus clock is 80 MHz 
  Random_Init(1);
  Output_Init();
	ADC_Init();
	DAC_Init();
	Sound_Init();
	SysTick_Init();
	Switch_Init();
  //Timer0_Init(&background,26666666); // 50 Hz
	
	ST7735_FillScreen(ST7735_BLACK);
	ST7735_SetCursor(3,1);
	ST7735_OutString((char*) "Duck Hunt: v1.0");
	ST7735_SetCursor(2, 3);
	ST7735_OutString((char*)"(poor man edition)");
	ST7735_SetCursor(3, 5);
	ST7735_OutString((char*)"PE2 for Spanish");
	ST7735_SetCursor(3, 7);
	ST7735_OutString((char*)"PE1 for English");
	ST7735_SetCursor(0, 0);
  while(GPIO_PORTE_DATA_R != 0x04 && GPIO_PORTE_DATA_R != 0x08){}
	if(GPIO_PORTE_DATA_R == 0x04){
		language = 0;
		Sound_Shoot();
	}else if(GPIO_PORTE_DATA_R == 0x08){
		language = 1;
		Sound_Shoot();
	}
  Timer1_Init(&clock,period); // 1 Hz
  sprite_t opps[20];
	EnableInterrupts();
	//
	//
	ST7735_FillScreen(ST7735_CYAN);
	if(language){
			ST7735_OutString((char*)"Tiros: ");
	} else{
			ST7735_OutString((char*)"Shots: ");
	}
	ST7735_OutUDec(shot_count);
	ST7735_SetCursor(0, 1);
	if(language){
			ST7735_OutString((char*)"Nivel: ");
	} else{
			ST7735_OutString((char*)"Level: ");
	}
	ST7735_OutUDec(level);
	ST7735_SetCursor(0, 2);
	if(language){
			ST7735_OutString((char*)"Puntos: ");
	} else{
			ST7735_OutString((char*)"Points: ");
	}
	ST7735_OutUDec(points);
	
  while(bill.life == alive){
    while(flag==0){
				Button_Press(duck.y, bill.y);
				if(shot_count == 0){ Game_Over();}
				if(points == 5) {
						level++;
						points = 0;
						ST7735_SetCursor(7,1);
						ST7735_OutUDec(level);
						ST7735_SetCursor(8,2);
						ST7735_OutUDec(points);
						period = period - 5000000;
						Timer1_Init(&clock,period);
					}
		}
    flag = 0;
		int32_t previous = bill.y; 
		bill.y = my.Distance() / 16 ;
		if(bill.y < 70) bill.y = 70;
    ST7735_DrawBitmap(bill.x,bill.y,bill.image,32,36);
		
			
		//redrawing sprites with backgrounds to prevent artifacting
		
		if (previous > bill.y){		
			ST7735_FillRect(0, bill.y, 32, 12, ST7735_CYAN);
		}
		else if (previous < bill.y ){		
			ST7735_FillRect(0, bill.y - 40, 32, 12, ST7735_CYAN);
		}
	}
/*
	bugs left to fix:
	 X 1. movement is inverted relative to slidepot, up is down and down is up (dk if i want to fix it tbh)
	X 2. button1 continues to have a value of one despite no longer being pressed. it's updating in the background but only with nonzero values. thus it can't enter case 2 
	where the flag is set but the button is pressed. thus the flag never gets reset to 0.
	X 3. this issue affects the level and shot counters, as the collision loop can't be entered more than once
	
	features left to implement:
	1. sound (duh)
	2. different levels, each level should gain difficulty (aka shorten the reload period)
	X 3. pause screen for button2
	X 4. game over screen for when shot counter equals 0. needs to be designed.
	5. display different languages
	
	*/
  
  
}
