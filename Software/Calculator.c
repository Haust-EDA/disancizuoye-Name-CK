#include "Calculator.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "tinyexpr.h"

#define USE_EXPR_

#define MAX_STACK_SIZE 20

char CalStrIn[STRIN_MAX_LENGTH] = ""; // 输入的字符串
uint8_t CalStrIn_Index = 0;			  // 输入字符串索引
double CalResult = 0;				  // 计算结果

char CalHStr[CAL_MAX_HISTORY][STRIN_MAX_LENGTH];
uint8_t CalHStrIndex[CAL_MAX_HISTORY];
double CalHRes[CAL_MAX_HISTORY];
uint8_t CalHIndex = 0; // 历史记录索引

/**
 * @brief 向CalStrIn中添加字符
 * @param c 要添加的字符
 * @return 0:成功 1:失败
 */
uint8_t Cal_StrInAdd(char c)
{
	if (CalStrIn_Index < STRIN_MAX_LENGTH - 1)
	{
		CalStrIn[CalStrIn_Index] = c;
		CalStrIn_Index++;
		CalStrIn[CalStrIn_Index] = '\0';
	}
	else
	{
		return 1;
	}
	return 0;
}
/**
 * @brief 删除CalStrIn中的最后一个字符
 * @return 0:成功 1:失败
 */
uint8_t Cal_StrInDel(void)
{
	if (CalStrIn_Index == 0)
	{
		return 1;
	}
	CalStrIn_Index--;
	CalStrIn[CalStrIn_Index] = '\0';
	return 0;
}

/**
 * @brief 重置计数器
 * @return 无
 */
void Cal_Exit(void)
{
	CalStrIn_Index = 0;
	CalStrIn[0] = '\0';
}

/**
 * @brief 添加历史记录
 */
uint8_t Cal_HistoryAdd(void)
{
	if (CalHIndex >= CAL_MAX_HISTORY)
	{
		for (uint8_t i = 1; i < CAL_MAX_HISTORY; i++)
		{
			strcpy(CalHStr[i - 1], CalHStr[i]);
			CalHStrIndex[i - 1] = CalHStrIndex[i];
			CalHRes[i - 1] = CalHRes[i];
		}
	}
	strcpy(CalHStr[CalHIndex], CalStrIn);
	CalHStrIndex[CalHIndex] = CalStrIn_Index;
	CalHRes[CalHIndex] = CalResult;
	CalHIndex = CalHIndex % CAL_MAX_HISTORY + 1;
	return CalHIndex;
}
/**
 * @brief 计算表达式
 * @return 0:失败;其它值表示历史索引
 */
uint8_t Cal_Run(void)
{
	double temp = te_interp(CalStrIn, 0);
	if (temp != NAN)
	{
		CalResult = temp;
		Cal_StrInAdd('=');
		return Cal_HistoryAdd();
	}
	else
	{
		return 0;
	}
}
