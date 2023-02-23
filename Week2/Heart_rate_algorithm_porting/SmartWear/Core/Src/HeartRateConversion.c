////////////////////////////////////////////////心率转换算法////////////////////////////////////////////////
#include "HeartRateConversion.h"

// these variables are volatile because they are used during the interrupt service routine!
#ifndef false
#define false  0
#endif

#ifndef true
#define true   1
#endif

int BPM;                   	            // 用于保存脉冲速率
unsigned char QS = false;               // 当检测到心率时，QS==true

static int Signal;               		// 持有的原始数据
static int IBI = 600;            	    // holds the time between beats, must be seeded!
static unsigned char Pulse = false;     // true when pulse wave is high, false when it's low
static int rate[10];                    // array to hold last ten IBI values
static unsigned long sampleCounter = 0; // used to determine pulse timing
static unsigned long lastBeatTime = 0;  // used to find IBI
static int P = 512;                     // used to find peak in pulse wave, seeded
static int T = 512;                     // used to find trough in pulse wave, seeded
static int thresh = 512;                // used to find instant moment of heart beat, seeded
static int amp = 100;                   // used to hold amplitude of pulse waveform, seeded
static unsigned char firstBeat = true;  // used to seed rate array so we startup with reasonable BPM
static unsigned char secondBeat = false;// used to seed rate array so we startup with reaso

/* 函数功能：心率转换算法
   形参说明：OriginalDat ADC采集到的原始数据
   使用说明：建议定时调用此函数(可以使用定时器中断) 频率：1ms一次
            全局变量BPM就是最终的转换结果(单位：次/分)
*/
void HeartRate_conversion(uint16_t _usOriginalDat)
{
    uint16_t runningTotal = 0;
    uint8_t i;
    uint16_t Num;
    //读取到的值右移2位，12位-->10位
    Signal = _usOriginalDat >> 2;   // read the Pulse Senso
    sampleCounter += 2;                          /* 用这个变量来跟踪ms的时间 */ 
    Num = sampleCounter - lastBeatTime;          /* 计算距离最后一次节拍的时间，以避免噪音 */ 

    /* 找出脉冲波的波峰和波谷 */
    if (Signal < thresh && Num > (IBI / 5) * 3)
    {
        /* 通过等待最后一次IBI的3/5来避免二色噪声 */
        if (Signal < T)
        {
            // T is the trough
            T = Signal;                           // keep track of lowest point in pulse wave
        }
    }
    if (Signal > thresh && Signal > P)
    {
        // thresh condition helps avoid noise
        P = Signal;                             // P is the peak
    }                                         // keep track of highest point in pulse wave
    
    /* 开始寻找心跳 */
    /* 每当有脉冲时，信号的值就会激增 */
    if (Num > 250)
    {
        /* 避免高频噪音 */
        if ((Signal > thresh) && (Pulse == false) && (Num > (IBI / 5) * 3))
        {
            Pulse = true;                               // set the Pulse flag when we think there is a pulse
            IBI = sampleCounter - lastBeatTime;         // measure time between beats in mS
            lastBeatTime = sampleCounter;               // keep track of time for next pulse
            if (secondBeat)
            {
                // if this is the second beat, if secondBeat == TRUE
                secondBeat = false;                  // clear secondBeat flag
                for (i = 0; i <= 9; i++)
                {
                    // seed the running total to get a realisitic BPM at startup
                    rate[i] = IBI;                     //rate[0]-rate[9]的值都一样吗？？？？
                }
            }
            if (firstBeat)
            {
                // if it's the first time we found a beat, if firstBeat == TRUE
                firstBeat = false;                   	// clear firstBeat flag
                secondBeat = true;                   	// set the second beat flag
                return;                              	// IBI value is unreliable so discard it
            }
            // keep a running total of the last 10 IBI values
            // runningTotal = 0;                  		// clear the runningTotal variable
            for (i = 0; i <= 8; i++)
            {
                // shift data in the rate array
                rate[i] = rate[i + 1];                // and drop the oldest IBI value
                runningTotal += rate[i];              // add up the 9 oldest IBI values
            }
            rate[9] = IBI;                          // add the latest IBI to the rate array
            runningTotal += rate[9];                // add the latest IBI to runningTotal
            runningTotal /= 10;                     // average the last 10 IBI values
            BPM = 60000 / runningTotal;             // how many beats can fit into a minute? that's BPM!
            QS = true;                              // set Quantified Self flag
            // QS FLAG IS NOT CLEARED INSIDE THIS ISR
        }
    }
    //脉冲开始下降
    if (Signal < thresh && Pulse == true)
    {
        // when the values are going down, the beat is over
        Pulse = false;                         // reset the Pulse flag so we can do it again
        amp = P - T;                           // get amplitude of the pulse wave
        thresh = amp / 2 + T;                  // set thresh at 50% of the amplitude
        P = thresh;                            // reset these for next time
        T = thresh;
    }
    //没有检测到脉冲，设置默认值
    if (Num > 2500)
    {
        // if 2.5 seconds go by without a beat
        thresh = 512;                          // set thresh default
        P = 512;                               // set P default
        T = 512;                               // set T default
        lastBeatTime = sampleCounter;          // bring the lastBeatTime up to date
        firstBeat = true;                      // set these to avoid noise
        secondBeat = false;                    // when we get the heartbeat back
    }
}
