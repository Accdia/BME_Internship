#ifndef _HeartRate_H_
#define _HeartRate_H_

#include "main.h"

extern int BPM;                   			 // 用于保存脉冲速率
extern unsigned char QS;

void HeartRate_conversion(uint16_t OriginalDat);

#endif
