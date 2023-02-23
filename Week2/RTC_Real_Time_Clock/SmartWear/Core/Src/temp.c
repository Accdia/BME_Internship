#include "temp.h"
#include "usart.h"

void TEMP_Init(void)
{
    /* 自动读取温度指令 */
    uint8_t cmd[3] = {0xA5, 0x45, 0xEA};
    /* 发送指令 */
    HAL_UART_Transmit(&huart2, &cmd[0], 1, 0xFFFF);
    HAL_UART_Transmit(&huart2, &cmd[1], 1, 0xFFFF);
    HAL_UART_Transmit(&huart2, &cmd[2], 1, 0xFFFF);
}

uint8_t Receive_ok;
uint8_t rebuf[20] = {0};
/* 接收数据 */
void RxTempInfo(uint8_t tmp)
{
    static uint8_t i = 0;
  
    rebuf[i++] = tmp; //读取串口数据，同时清接收标志
    if (rebuf[0] != 0x5a) //帧头不对
        i = 0;
    if ((i == 2) && (rebuf[1] != 0x5a)) //帧头不对
        i = 0;
    if (i > 3) //i 等于 4 时，已经接收到数据量字节 rebuf[3]
    {
        if (i != (rebuf[3] + 5)) //判断是否接收一帧数据完毕
            return ;
        switch (rebuf[2]) //接收完毕后处理
        {
        case 0x45:
            if (!Receive_ok) //当数据处理完成后才接收新的数据
            {
                Receive_ok = 1; //接收完成标志
            }
            break;
        case 0x15:
            break;
        case 0x35:
            break;
        }
        i = 0; //缓存清 0
    }
}

/* 获取温度 */
void GetTempInfo(void)
{
    if (Receive_ok)
    {
      float TO = 0, TA = 0;
      uint8_t sum = 0, i = 0;
      for (sum = 0, i = 0; i < (rebuf[3] + 4); i++)
          sum += rebuf[i];
      if (sum == rebuf[i]) //校验和判断
      {
          TO = (float)((rebuf[4] << 8) | rebuf[5]) / 100; //得到真实温度
          TA = (float)((rebuf[6] << 8) | rebuf[7]) / 100; //得到真实温度
      }
      printf("目标温度TO: %f\r\n", TO);
      printf("环境温度TA: %f\r\n", TA);
      Receive_ok = 0;
    }
}
