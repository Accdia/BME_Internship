#include "key.h"

/*
*********************************************************************************************************
*	函数原型: uint8_t KEY_Scan(void)
*	函数功能: 按键扫描
*	函数形参: 无
*	返 回 值: 按键键值
                0  没有有效按键
                1  KEY_UP
                2  KEY2
*********************************************************************************************************
*/
uint8_t KEY_Scan(void)
{
    /* 记录按键状态,防止对单一按键重复响应 */
    static uint8_t ucFlag = 0;    /* 0,没按下(没处理) 1,已经按下(已经处理)*/
    
    /* 获取按键状态,并判断按键是否按下 */
    if ((ucFlag == 0) && (KEY_UP || KEY2)) 
    {
        /* 延时消抖 */
        HAL_Delay(15);
        
        /* 标记一下 */
        ucFlag = 1;
        
        /* 再次判断按键状态 */
        if (KEY_UP) return KEY_UP_PRES;
        else if (KEY2)  return KEY2_PRES;
    }
    else if (!KEY_UP && !KEY2)
    {
        ucFlag = 0;     /* 当所有按键松开时,恢复标记初始状态 */
    }
    return 0;  
}
