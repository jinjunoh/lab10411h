// put prototypes for public functions, explain what it does
// put your names here, date
#ifndef __IO_H__ // do not include more than once
#define __IO_H__
#include <stdint.h>
void IO_Init(void);
void IO_HeartBeat(void);
void IO_Touch(void);
void Wait10ms(uint32_t count);

#endif
