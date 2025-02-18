#include "Stopwatch.h"
#include "stdint.h"
extern uint32_t TimeTick;

uint8_t SW_Flag = 0; // 计时状态

uint32_t SW_Time0Start = 0;
uint32_t SW_Time0 = 0; // 当前计时


uint32_t SW_Times[SW_TIMESMAX] = {0}; // 计时记录
uint8_t SW_TimesIndex = 0;            // 计时记录索引

/**
 * @brief 开始计时
 */
void SW_Start(void)
{
	SW_Flag = 1; // 进行计时标志
	SW_Time0 = 0;
	SW_Time0Start = TimeTick;
}
/**
 * @brief 复位
 */
void SW_Reset(void)
{
	SW_Flag = 0; // 停止计时标志
	SW_TimesIndex = 0;
	SW_Time0 = 0;
}

/**
 * @brief 暂停/继续
 */
void SW_Pause(void)
{
	if (SW_Flag == 1) // 处于计时状态
	{
		SW_Time0 = TimeTick - SW_Time0Start; // 刷新时间
		SW_Flag = 2;                         // 暂停计时标志
	}
	else if (SW_Flag == 2) // 处于暂停状态
	{
		SW_Time0Start = TimeTick - SW_Time0;
		SW_Flag = 1; // 继续计时标志
	}
}
/**
 * @brief 刷新当前计时
 */
void SW_TimeRe(void)
{
	if (SW_Flag == 1) // 进行计时
	{
		SW_Time0 = TimeTick - SW_Time0Start;
	}
}

/**
 * @brief 记录当前计时
 * @details 记录当前计时到SW_Times数组中，并返回计时记录索引;
 *          如果记录数组已满，则覆盖最早的记录;
 * @return 计时记录索引
 */
uint8_t SW_Record(void)
{
	if (SW_TimesIndex < SW_TIMESMAX)
	{
		SW_Time0 = TimeTick - SW_Time0Start;
		SW_Times[SW_TimesIndex++] = SW_Time0;
	}
	else
	{
		for (uint8_t i = 0; i < SW_TIMESMAX - 1; i++)
		{
			SW_Times[i] = SW_Times[i + 1];
		}
		SW_Time0 = TimeTick - SW_Time0Start;
		SW_Times[SW_TimesIndex - 1] = SW_Time0;
	}
	return SW_TimesIndex;
}
/**
 * @brief 格式化时间(毫秒 -> 分，秒，毫秒)
 */
void SW_Timef(uint32_t time, uint8_t *min, uint8_t *sec, uint8_t *msec)
{
	*min=time/ 1000 / 60;
	*sec=time/ 1000 % 60;
	*msec = time % 1000 / 10;
}
