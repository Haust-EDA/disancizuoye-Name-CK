/**
 * 				PB4	PB5	PB6	PB7
 *				C1	C2	C3	C4
 *	PA11	R1	1	2	3	4
 *	PA10	R2	5	6	7	8
 *	PA9		R3	9	10	11	12
 *	PA8		R4	13	14	15	16
 *
 *
 *
 **/

#include "stm32f10x.h" // Device header

#define KEYSETTIME 350 // 长按最低时间

// 管脚定义
#define C1 GPIO_Pin_4 // GPIOB
#define C2 GPIO_Pin_5 // GPIOB
#define C3 GPIO_Pin_8 // GPIOB
#define C4 GPIO_Pin_9 // GPIOB

#define R1 GPIO_Pin_11 // GPIOA
#define R2 GPIO_Pin_10 // GPIOA
#define R3 GPIO_Pin_9  // GPIOA
#define R4 GPIO_Pin_8  // GPIOA

const uint16_t KCols[] = {C1, C2, C3, C4}; // 列GPIO
const uint16_t KRows[] = {R1, R2, R3, R4}; // 行GPIO
#define KNumCol (sizeof(KCols) / sizeof(KCols[0]))
#define KNumRow (sizeof(KRows) / sizeof(KRows[0]))

/* 全局变量 */
uint8_t Key_Num = 0;
uint8_t Key_State = 0;	  // 按键状态(0:无按键,1:短按,2:长按)
uint16_t Key_Time = 0;	  // 按键按下的时间
static uint8_t CurrState; // 按键状态:当前，
static uint8_t PrevState; // 按键状态:前一刻

/**
 * @brief 初始化按键GPIO
 */
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
 * @brief 取键值
 * @details 返回键值和状态后，清空按键值、状态和时间
 * @param State 按键状态(0:无按键,1:短按,2:长按)
 * @retval 0	NULL
 **/
uint8_t Key_GetNum(uint8_t *State)
{
	// 键值
	static uint8_t Temp;
	Temp = Key_Num;
	Key_Num = 0;
	// 状态
	*State = Key_State;
	if (Temp != 0 && Key_State != 1)
	{
		Key_Time = 0;
		Key_State = 0;
	}

	return Temp;
}
/**
 * @brief 按键扫描(行)
 * @param Row 行号(0-3)
 * @retval 按键值
 */
uint8_t Key_ScanRow(uint8_t Row)
{
	uint8_t Temp = Row * KNumCol + 1;
	uint8_t Key_ScanNum = 0;

	GPIO_ResetBits(GPIOA, KRows[Row]);
	for (uint8_t i = 0; i < KNumCol; i++) // 遍历所有列
	{
		if (GPIO_ReadInputDataBit(GPIOB, KCols[i]) == 0)
		{
			Key_ScanNum = Temp + i;
			break;
		}
	}
	GPIO_SetBits(GPIOA, KRows[Row]);
	return Key_ScanNum;
}

/**
 * @brief 获取当前时刻的按键状态
 * @return 按键值
 */
uint8_t Key_GetState(void)
{
	uint8_t Temp = 0;

	if (PrevState != 0) // 如果前一刻是按下状态，则检测对应行
	{
		Temp = Key_ScanRow(PrevState / KNumCol);
	}
	if (Temp != 0)
	{
		return Temp;
	}

	for (uint8_t i = 0; i < KNumRow; i++) // 遍历所有行
	{
		Temp = Key_ScanRow(i);
		if (Temp != 0)
		{
			return Temp;
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
		CurrState = Key_GetState(); // 获取当前时刻的键值

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
			Key_Time = 0;
		}
	}
}
