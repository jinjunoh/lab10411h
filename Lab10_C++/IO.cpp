// IO.c
// This software configures the switch and LED
// You are allowed to use any switch and any LED, 
// although the Lab suggests the SW1 switch PF4 and Red LED PF1
// Runs on TM4C123
// Program written by: put your names here
// Date Created: 
// Last Modified:  August 24, 2022
// Lab number: 7


#include "../inc/tm4c123gh6pm.h"
#include <stdint.h>
#include "IO.h"

void Wait10ms(uint32_t);

//------------IO_Init------------
// Initialize GPIO Port for a switch and an LED
// Input: none
// Output: none
void IO_Init(void) { 
	volatile uint32_t delay;
 // --UUU-- Code to initialize PF4 and PF2
	SYSCTL_RCGCGPIO_R |= 0x30;
	Wait10ms(delay);
	
	
	GPIO_PORTE_DIR_R |= 0x03;
	GPIO_PORTE_DEN_R |= 0x03;
}

//------------IO_HeartBeat------------
// Toggle the output state of the  LED.
// Input: none
// Output: none
void IO_HeartBeat(void) {
 // --UUU-- PF2 is heartbeat
		GPIO_PORTF_DATA_R ^= 0x4;
}


//------------IO_Touch------------
// wait for release and press of the switch
// Input: none
// Output: none
void IO_Touch(void) {
 // --UUU-- wait for release; delay for 20ms; and then wait for press

	while((GPIO_PORTE_DATA_R&0x10) == 0x10) {}
	
	Wait10ms(2);
	
	while((GPIO_PORTE_DATA_R&0x10) != 0x10) {}
	
}  

//Must really wait for 10ms x count
void Wait10ms(uint32_t count){
  uint32_t i;
  for(i=0; i <count*32000; i++){}
}
