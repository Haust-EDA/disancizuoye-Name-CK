#ifndef _KEY_H_
#define _KEY_H_
#include "stm32f10x.h"                  // Device header

void Key_Init(void);
uint8_t Key_GetNum(uint8_t *State);//获取键码

void Key_Tick(void);//更新键码;将该函数加入定时器中断函数

#endif
