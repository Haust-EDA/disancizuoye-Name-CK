/**
 * 使用TIM3，定时周期1ms
 * 使用时请再主程序中添加函数void TIM3_IRQHandler(void){}//模版在下方
 * 通过这种方式修改定时器的具体功能
 */

#include "stm32f10x.h"                  // Device header

void Timer_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	
	/*配置时钟*/
	TIM_InternalClockConfig(TIM3);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;
	//CK_CNT_OV=CK_PSC/(PSC+1)/(ARR+1)
	//72 000 000 /7200/10000
	TIM_TimeBaseInitStruct.TIM_Prescaler=72-1;
	TIM_TimeBaseInitStruct.TIM_Period=1000-1;//2000 Hz
	
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter=0;
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStruct);//为更新预分频器和重复计数器的值，该函数会直接进入一次中断
	TIM_ClearFlag(TIM3,TIM_FLAG_Update);//清除标志位
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);//使能中断，当计数达到设定值时，触发中断处理程序。
	
	
	/*配置中断*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=2;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM3,ENABLE);//启动定时器，定时器开始计数。
}
/*
 * TIM3定时器，定时中断函数，周期1ms
 
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET)
	{
		//功能代码区

		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);//清除中断标志位
	}
}

*/
