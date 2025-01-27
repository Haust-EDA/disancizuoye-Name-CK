#include "stm32f10x.h" // Device header
#include "Delay.h"
#include "LCD.h"
#include "Timer.h"
#include "Key.h"
#include "LED.h"
#include "Control.h"

uint32_t TimeTick = 0;
uint8_t KeyNum = 0, KeyState = 0;
uint16_t FPSCounter = 0, FPS = 0;

void Init(void)
{
	LCD_Init();
	Timer_Init();
	Key_Init();
	LED_Init();

	LCD_Fill(0, 0, 160, 128, WHITE);
	C_MenuShow();
}
extern uint8_t LEDIndex;
int main(void)
{
	extern uint8_t CalHIndex;
	Init();
	while (1)
	{
		// LCD_ShowIntNum(147, 115 * 0, TimeTick / 100, 2, BLACK, WHITE, 12);
		// LCD_ShowString(160 - 6 * 8, 0, Strf("F%03d T%02d", FPS, (TimeTick / 100) % 100), BLACK, WHITE, 12, 0);
		// LCD_ShowString(160 - 6 * (8 + 4), 0, Strf("CS%d", CalHIndex), BLACK, WHITE, 12, 0);
		// 刷新键值
		KeyNum = Key_GetNum(&KeyState);
		if (KeyNum != 0)
		{
			// LCD_ShowIntNum(0, 0, KeyNum, 2, BLACK, WHITE, 16); // 显示按键状态
			// LCD_ShowIntNum(24, 0, KeyState, 2, BLACK, WHITE, 16);
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
		FPSCounter++;
	}
}

void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)
	{
		// 功能代码区
		Key_Tick();
		TimeTick++;
		if (TimeTick % 1000 == 0)
		{
			FPS = FPSCounter;
			FPSCounter = 0;
		}

		TIM_ClearITPendingBit(TIM3, TIM_IT_Update); // 清除中断标志位
	}
}
