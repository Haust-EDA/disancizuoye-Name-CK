#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Timer.h"
#include "Key.h"

uint16_t Num;//从外部引用变量
uint8_t State;
uint16_t Time;
int main(void)
{

	//LED_Init();
	OLED_Init();
	Timer_Init();
	Key_Init();
	OLED_ShowString(1,1,"Num=");
	while(1)
	{
		
		Num=Key_GetNum(&State);
		if(Num!=0)
		{
			OLED_ShowNum(1,5,Num,2);
			OLED_ShowNum(2,1,State,2);
		}
		
		
		OLED_ShowNum(1,10,Time,5);
	}
	
}
/**
 * @brief TIM2定时器，定时中断函数，周期1ms
 */
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET)
	{
		//功能代码区
		Key_Tick();
		Time++;
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);//清除中断标志位
	}
}
