#include "stm32f10x.h" // Device header
#include "GType.h"
/* 全局变量 */
UIState G_UIState=UIT_MenuMain;

uint8_t G_Choice_Index = 1;// 当前选项(1-4)
uint8_t G_KeyNum = 0;// 当前键值

