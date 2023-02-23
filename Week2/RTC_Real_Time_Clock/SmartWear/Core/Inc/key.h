#ifndef _KEY_H_
#define _KEY_H_

#include "main.h"
#include "gpio.h"

/* 宏定义:获取按键对应引脚此时的电平状态 */
#define KEY_UP  HAL_GPIO_ReadPin(KEY_UP_GPIO_Port, KEY_UP_Pin)  
#define KEY2    !(HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin))

/* 定义键值 */
#define KEY_UP_PRES 1
#define KEY2_PRES   2

/* 函数声明 */
uint8_t KEY_Scan(void);

#endif
