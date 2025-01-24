#ifndef __LED_H_
#define __LED_H_
#include "stm32f10x.h"

extern uint8_t LEDCMode;  // 流水灯模式(0:常闭;1;2;3;4;5:常亮)
extern uint8_t LEDCDir;   // 流水灯方向(0:顺;1:逆)
extern uint8_t LEDCSpeed; // 流水灯速度(*100ms)
extern uint8_t LEDBMode;  // 呼吸灯模式(0:关闭;1:同步;2:跟随)

void LED_Init(void);
void LED_Tick(void);
void LED_Refresh(void);

void LED_SetCMode(uint8_t mode);
void LED_SetCDir(uint8_t dir);
void LED_SetCSpeed(uint8_t speed);
void LED_SetBMode(uint8_t mode);

#endif
