#ifndef __STOPWATCH_H_
#define __STOPWATCH_H_
#include "stdint.h"

#define SW_TIMESMAX 15                // 最大记录数

extern uint8_t SW_Flag;
extern uint32_t SW_Time0;
extern uint8_t SW_TimesIndex;
extern uint32_t SW_Times[SW_TIMESMAX];


void SW_Start(void);//开始
void SW_Reset(void);//复位
void SW_Pause(void);//暂停/继续
uint8_t SW_Record(void);//记录

void SW_TimeRe(void);//时间刷新
void SW_Timef(uint32_t time, uint8_t *min, uint8_t *sec, uint8_t *msec);




#endif
