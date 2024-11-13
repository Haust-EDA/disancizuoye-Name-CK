/**
 * 定时器型非阻塞按键；需配合定时器使用,将Key_Tick();放入定时器的中断函数中
 * 
 * 使用GPIOB的Pin12,13口作为输入（上拉）
 * Key1->GPIO_Pin_12 	: 1键
 * Key2->GPIO_Pin_13 	: 2键
 **/

#include "stm32f10x.h"                  // Device header
#include "Delay.h"

//管脚定义
#define Key1 GPIO_Pin_12//1
#define Key2 GPIO_Pin_13//2
static uint8_t Key_Num=0;


void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin=Key1|Key2;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}


/**
 *@brief 取键值
 *@retval 0->NULL;1->Key1;2->Key2
 **/
uint8_t Key_GetNum(void)
{
	uint8_t Temp;
	Temp = Key_Num;
	Key_Num = 0;
	return Temp;
}
/**
 * @brief 获取当前时刻的按键状态
 */
uint8_t Key_GetState(void)
{
	if (GPIO_ReadInputDataBit(GPIOB,Key1) == 0)
	{
		return 1;
	}
	if (GPIO_ReadInputDataBit(GPIOB, Key2) == 0)
	{
		return 2;
	}
	return 0;
}
/**
 * @brief 更新键值；接入定时器(1ms)
 */
void Key_Tick(void)
{
	static uint8_t Count;//时间间隔计数
	static uint8_t CurrState, PrevState;//按键状态:当前时刻，上一时刻
	Count ++;

	if (Count >= 20)//20ms判断一次
	{
		Count = 0;//时间间隔计数清零
		CurrState = Key_GetState();//获取当前时刻
		if (CurrState == 0 && PrevState != 0)//按键状态变化
		{
			Key_Num = PrevState;
		}
		PrevState = CurrState;//转移到上一时刻
	}
}
