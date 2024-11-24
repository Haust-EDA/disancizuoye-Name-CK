#include "stm32f10x.h" // Device header
#include "Control.h"
#include "Gvar.h"
#include "Timer.h"
#include "OLED_UI.h"


void Init(void)
{
	/*硬件*/
	Timer_Init();
	Key_Init();
	OLED_Init();

	/*界面*/
	UI_MenuMain(&G_Choice_Index);
}

int main(void)
{
	Init();
	while (1)
	{
		Control_Main();
	}
}
void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		// 功能代码区
		Control_Tick();
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update); // 清除中断标志位
	}
}
