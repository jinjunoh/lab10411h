// put prototypes for public functions, explain what it does
// put your names here, date
#ifndef __DAC_H__ // do not include more than once
#define __DAC_H__
#include <stdint.h>

void DAC_Init(void);
void DAC_Out(uint32_t data);
void Switch_Init(void);
void Button_Press(int a, int b);
void Collision(int a, int b);
void Game_Over(void);
void Pause_Screen(void);
#endif
