#include "stm32f10x.h" // Device header
#include "Delay.h"
#include "LCD.h"
#include "Timer.h"
#include "Key.h"
#include "LED.h"

#include "Control.h"
uint32_t TimeTick = 0;
uint8_t KeyNum = 0, KeyState = 0;

void Init(void)
{
	LCD_Init();
	Timer_Init();
	Key_Init();
	LED_Init();

	LCD_Fill(0, 0, 160, 128, WHITE);
	C_MenuShow();
}

int main(void)
{
	Init();
	while (1)
	{
		LCD_ShowIntNum(147, 115 * 0, TimeTick / 100, 2, BLACK, WHITE, 12);
		// 刷新键值
		KeyNum = Key_GetNum(&KeyState);
		if (KeyNum != 0)
		{
			//LCD_ShowIntNum(0,0,KeyNum,2,BLACK,WHITE,24);// 显示按键状态
			//LCD_ShowIntNum(24,0,KeyState,2,BLACK,WHITE,24);
			switch (C_ShowIndex)
			{
			case 0:
				C_MenuKey();
				break;
			case 1:
				C_LEDKey();
				break;
			case 2:
				C_CalKey();
				break;
			case 3:
				C_SWKey();
				break;
			default:
				break;
			}
		}
		// 刷新
		C_Refresh();
	}
}

void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)
	{
		// 功能代码区
		Key_Tick();
		TimeTick++;

		TIM_ClearITPendingBit(TIM3, TIM_IT_Update); // 清除中断标志位
	}
}
