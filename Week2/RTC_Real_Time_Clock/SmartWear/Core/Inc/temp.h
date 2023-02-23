#ifndef __TEMP_H
#define __TEMP_H

#include "main.h"

extern uint8_t Receive_ok;
extern uint8_t rebuf[20];

void TEMP_Init(void);
void RxTempInfo(uint8_t tmp);
void GetTempInfo(void);

#endif
