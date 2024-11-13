#include "stm32f10x.h"                  // Device header
#include "Timer.h"
#include "Key.h"
#include "OLED.h"

void Init(void)
{
	/*硬件*/
	Timer_Init();
	Key_Init();
	OLED_Init();
	
	/*界面*/
	OLED_ShowChar(1,1,'a');
	OLED_ShowString(2,1,"Hello World!");
}

int main(void)
{
	Init();
	while(1)
	{
		
	}
}
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET)
	{
		//功能代码区
		Key_Tick();//刷新按键
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);//清除中断标志位
	}
}


