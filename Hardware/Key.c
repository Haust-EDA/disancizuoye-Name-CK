/**
 *使用GPIOB的Pin12,13口作为输入（上拉）
 *Key1->GPIO_Pin_11 	: 1键
 *Key2->GPIO_Pin_1 		: 2键
 **/

#include "stm32f10x.h" // Device header

#define KEYSETTIME 400 // 长按最低时间

// 管脚定义
#define C1 GPIO_Pin_4 // GPIOB
#define C2 GPIO_Pin_5 // GPIOB
#define C3 GPIO_Pin_6 // GPIOB
#define C4 GPIO_Pin_7 // GPIOB

#define R1 GPIO_Pin_11 // GPIOA
#define R2 GPIO_Pin_10 // GPIOA
#define R3 GPIO_Pin_9  // GPIOA
#define R4 GPIO_Pin_8  // GPIOA

/* 全局变量 */
uint8_t Key_Num = 0;
uint8_t Key_State = 0;				 // 按键状态(0:无按键,1:短按,2:长按)
uint16_t Key_Time = 0;				 // 按键按下的时间
static uint8_t CurrState, PrevState; // 按键状态:当前，前一刻

void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;

	// C
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // 上拉输入
	GPIO_InitStructure.GPIO_Pin = C1 | C2 | C3 | C4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// R
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // 推挽输出
	GPIO_InitStructure.GPIO_Pin = R1 | R2 | R3 | R4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA, R1 | R2 | R3 | R4);
}

/**
 *@brief 取键值
 *@retval 0->NULL;1->Key1;2->Key2
 **/
uint8_t Key_GetNum(uint8_t *State)
{
	// 键值
	static uint8_t Temp;
	Temp = Key_Num;
	Key_Num = 0;
	// 状态
	*State = Key_State;
	if (Temp != 0 && Key_State !=1)
	{
		Key_Time = 0;
		Key_State = 0;
	}
	
	return Temp;
}
/**
 * @brief 按键扫描
 */
uint8_t Key_ScanRow(uint16_t GPIO_Pin_X)
{
	uint8_t Temp = 0, Key_ScanNum = 0;
	switch (GPIO_Pin_X)
	{
	case R1:
		Temp = 1;
		break;
	case R2:
		Temp = 5;
		break;
	case R3:
		Temp = 9;
		break;
	case R4:
		Temp = 13;
		break;
	default:
		break;
	}
	GPIO_ResetBits(GPIOA, GPIO_Pin_X);
	if (GPIO_ReadInputDataBit(GPIOB, C1) == 0)
	{
		Key_ScanNum = Temp;
	}
	else if (GPIO_ReadInputDataBit(GPIOB, C2) == 0)
	{
		Key_ScanNum = Temp + 1;
	}
	else if (GPIO_ReadInputDataBit(GPIOB, C3) == 0)
	{
		Key_ScanNum = Temp + 2;
	}
	else if (GPIO_ReadInputDataBit(GPIOB, C4) == 0)
	{
		Key_ScanNum = Temp + 3;
	}
	GPIO_SetBits(GPIOA, GPIO_Pin_X);
	return Key_ScanNum;
}

/**
 * @brief 获取当前时刻的按键状态
 */
uint8_t Key_GetState(void)
{
	uint8_t Temp = 0;
	if (PrevState != 0)
	{
		if (PrevState > 0 && PrevState < 4)
		{
			Temp = Key_ScanRow(R1);
		}
		else if (PrevState > 3 && PrevState < 7)
		{
			Temp = Key_ScanRow(R2);
		}
		else if (PrevState > 6 && PrevState < 10)
		{
			Temp = Key_ScanRow(R3);
		}
		else if (PrevState > 9 && PrevState < 14)
		{
			Temp = Key_ScanRow(R4);
		}
	}
	if (Temp != 0)
	{
		return Temp;
	}
	for (uint8_t i = 0; i < 4; i++)
	{
		switch (i)
		{
		case 0:
			Temp = Key_ScanRow(R1);
			if (Temp != 0)
			{
				return Temp;
			}
			break;
		case 1:
			Temp = Key_ScanRow(R2);
			if (Temp != 0)
			{
				return Temp;
			}
			break;
		case 2:
			Temp = Key_ScanRow(R3);
			if (Temp != 0)
			{
				return Temp;
			}
			break;
		case 3:
			Temp = Key_ScanRow(R4);
			if (Temp != 0)
			{
				return Temp;
			}
			break;
		default:
			break;
		}
	}
	return Temp;
}
/**
 * @brief 更新键值；接入定时器(1ms)
 */
void Key_Tick(void)
{
	static uint8_t Count1; // 时间间隔计数
	Count1++;
	if (Count1 >= 20) // 20ms判断一次
	{
		Count1 = 0; // 时间间隔计数清零
		PrevState = CurrState;
		CurrState = Key_GetState();
		if (PrevState == CurrState && PrevState != 0) // 按键状态持续且按下
		{
			Key_State = 1; // 短按
			Key_Time++;
		}
		if (Key_Time > KEYSETTIME / 20)
		{
			Key_State = 2; // 长按
		}
		if (CurrState == 0 && PrevState != 0) // 按键状态变化(现在松手而之前按下)
		{
			Key_Num = PrevState;
		}
	}
}
