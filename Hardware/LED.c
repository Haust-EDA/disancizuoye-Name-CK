#include "LED.h"
#include "stm32f10x.h"
/*
	流水灯模式：常闭，1,2,3,4，常亮
		方向
		速度
	呼吸灯模式：同步，跟随
 */
#define GPIN_LED1 GPIO_Pin_0
#define GPIN_LED2 GPIO_Pin_1
#define GPIN_LED3 GPIO_Pin_10
#define GPIN_LED4 GPIO_Pin_11
#define GPIN_LED5 GPIO_Pin_12
#define GPIN_LED6 GPIO_Pin_13
#define GPIN_LED7 GPIO_Pin_14
#define GPIN_LED8 GPIO_Pin_15

#define LEDOC1(x) TIM_SetCompare1(TIM2, x)
#define LEDOC2(x) TIM_SetCompare2(TIM2, x)
#define LEDOC3(x) TIM_SetCompare3(TIM2, x)
#define LEDOC4(x) TIM_SetCompare4(TIM2, x)

extern uint32_t TimeTick;
uint32_t TimeStart = 0;

uint8_t LEDIndex = 1;

uint8_t LEDCMode = 0;  // 流水灯模式(0:常闭;1;2;3;4;5:常亮)
uint8_t LEDCDir = 0;   // 流水灯方向(0:顺;1:逆)
uint8_t LEDCSpeed = 5; // 流水灯速度(*100ms)
uint8_t LEDBMode = 0;  // 呼吸灯模式(0:关闭;1:同步;2:跟随)

void LED_PWMInit(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	/*配置 时钟*/
	TIM_InternalClockConfig(TIM2);

	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	// CK_CNT_OV=CK_PSC/(PSC+1)/(ARR+1)
	TIM_TimeBaseInitStruct.TIM_Prescaler = 720 - 1;
	TIM_TimeBaseInitStruct.TIM_Period = 100 - 1;
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;

	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct); // 为更新预分频器和重复计数器的值，该函数会直接进入一次中断
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);			 // 清除标志位

	/*配置 输出比较单元*/
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure); // 先初始化，再配置
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;	  // 极性
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // 使能
	TIM_OCInitStructure.TIM_Pulse = 0;							  // CCR的值

	TIM_OC1Init(TIM2, &TIM_OCInitStructure);
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);
	TIM_OC3Init(TIM2, &TIM_OCInitStructure);
	TIM_OC4Init(TIM2, &TIM_OCInitStructure);

	/*配置 GPIO*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 使用推挽输出，接入外设
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	TIM_Cmd(TIM2, ENABLE); // 启动定时器，定时器开始计数。
}
void LED_GPIOInit(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); // 使能B端口时钟
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // 设置为推挽输出模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // 设置为50MHz

	GPIO_InitStructure.GPIO_Pin = GPIN_LED1 | GPIN_LED2 | GPIN_LED3 | GPIN_LED4 | GPIN_LED5 | GPIN_LED6 | GPIN_LED7 | GPIN_LED8; // 选择要控制的引脚

	GPIO_Init(GPIOB, &GPIO_InitStructure); // 初始化GPIOB

	GPIO_SetBits(GPIOB, GPIN_LED1 | GPIN_LED2 | GPIN_LED3 | GPIN_LED4 | GPIN_LED5 | GPIN_LED6 | GPIN_LED7 | GPIN_LED8); // 全部熄灭
}

/**
 * @brief  初始化LED
 */
void LED_Init(void)
{
	LED_PWMInit();
	LED_GPIOInit();
}
/**
 * @brief  设置LED状态
 * @param  index LED序号
 * @param  state LED状态(0关;1开)
 */
void LED_Set(uint8_t index, uint8_t state)
{
	state = ~state;
	switch (index)
	{
	case 1:
		GPIO_WriteBit(GPIOB, GPIN_LED1, (BitAction)state);
		break;
	case 2:
		GPIO_WriteBit(GPIOB, GPIN_LED2, (BitAction)state);
		break;
	case 3:
		GPIO_WriteBit(GPIOB, GPIN_LED3, (BitAction)state);
		break;
	case 4:
		GPIO_WriteBit(GPIOB, GPIN_LED4, (BitAction)state);
		break;
	case 5:
		GPIO_WriteBit(GPIOB, GPIN_LED5, (BitAction)state);
		break;
	case 6:
		GPIO_WriteBit(GPIOB, GPIN_LED6, (BitAction)state);
		break;
	case 7:
		GPIO_WriteBit(GPIOB, GPIN_LED7, (BitAction)state);
		break;
	case 8:
		GPIO_WriteBit(GPIOB, GPIN_LED8, (BitAction)state);
		break;
	default:
		break;
	}
}
/**
 * @brief  设置LED PWM输出
 * @param  index LED序号
 * @param  value PWM输出值(0~100)
 * @return 无
 */
void LED_SetOC(uint8_t index, uint8_t value)
{
	switch (index)
	{
	case 1:
		LEDOC1(value);
		break;
	case 2:
		LEDOC2(value);
		break;
	case 3:
		LEDOC3(value);
		break;
	case 4:
		LEDOC4(value);
		break;
	default:
		break;
	}
}

/**
 * @brief LED状态调整
 */
void LED_Refresh(void)
{
	if (LEDCMode == 0 || LEDCMode == 5)
	{
		return;
	}
	uint16_t time = TimeTick - TimeStart;
	if (time >= LEDCSpeed * 100)
	{
		TimeStart = TimeTick;
		if (LEDCDir == 0)
		{
			LEDIndex--;
		}
		else
		{
			LEDIndex++;
		}
		if (LEDIndex > 8)
		{
			LEDIndex -= 8;
		}
		else if (LEDIndex < 1)
		{
			LEDIndex += 8;
		}
	}
	// 流水灯设置
	if (LEDCDir == 0)
	{
		for (uint8_t i = 0; i < LEDCMode; i++) // 设置亮
		{
			LED_Set((LEDIndex + i) % 8 + 1, 1);
		}
		for (uint8_t i = 0; i < (8 - LEDCMode); i++) // 设置灭
		{
			LED_Set((LEDIndex - i) % 8 + 1, 0);
		}
	}
	else if (LEDCDir == 1)
	{
		for (uint8_t i = 0; i < LEDCMode; i++) // 设置亮
		{
			LED_Set((LEDIndex - i) % 8 + 1, 1);
		}
		for (uint8_t i = 0; i < (8 - LEDCMode); i++) // 设置灭
		{
			LED_Set((LEDIndex + i) % 8 + 1, 0);
		}
	}
	// 呼吸灯设置
	uint8_t ccr = 0;
	switch (LEDBMode)
	{
	case 0: // 关闭
		for (uint8_t i = 1; i <= 4; i++)
		{
			LED_SetOC(i, 0);
		}
		break;
	case 1: // 同步

		if (time < (LEDCSpeed * 100 / 2))
		{
			ccr = 100 * (time / (LEDCSpeed * 100 / 2));
		}
		else
		{
			ccr = 100 - 100 * (time / (LEDCSpeed * 100 / 2));
		}
		for (uint8_t i = 1; i <= 4; i++)
		{
			LED_SetOC(i, ccr);
		}
		break;
	case 2: // 跟随
		for (uint8_t i = 0; i < LEDCMode; i++)
		{
			uint8_t ccr = 100 * (time / (LEDCSpeed * 100)) * (LEDCMode - i) / LEDCMode;
			uint8_t index = (LEDCDir == 1) ? (LEDIndex + i) % 4 + 1 : (LEDIndex - i) % 4 + 1;
			LED_SetOC(index, ccr);
		}
		break;
	default:
		break;
	}
}

void LED_SetCMode(uint8_t mode)
{
	LEDCMode = mode;
	if (LEDCMode == 0)
	{
		for (uint8_t i = 1; i <= 8; i++)
		{
			LED_Set(i, 0);
		}
	}
	else if (LEDCMode == 5)
	{
		for (uint8_t i = 1; i <= 8; i++)
		{
			LED_Set(i, 1);
		}
	}
}

void LED_SetCDir(uint8_t dir)
{
	LEDCDir = dir;
}
void LED_SetCSpeed(uint8_t speed)
{
	LEDCSpeed = speed;
}
void LED_SetBMode(uint8_t mode)
{
	LEDBMode = mode;
}
