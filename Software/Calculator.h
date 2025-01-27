#ifndef __CALCULATOR_H_
#define __CALCULATOR_H_
#include "stdint.h"

#define STRIN_MAX_LENGTH 10 * 4 // 最大输入字符串长度
#define CAL_MAX_HISTORY 15      // 最大历史记录数

extern char CalStrIn[STRIN_MAX_LENGTH]; // 输入的字符串(表达式)
extern uint8_t CalStrIn_Index;          // 输入字符串索引
extern double CalResult;                // 计算结果

extern char CalHStr[CAL_MAX_HISTORY][STRIN_MAX_LENGTH]; // 表达式历史记录
extern uint8_t CalHStrIndex[CAL_MAX_HISTORY];           // 表达式字符索引历史记录
extern double CalHRes[CAL_MAX_HISTORY];                 // 结果历史记录
extern uint8_t CalHIndex;                               // 历史记录索引

uint8_t Cal_StrInAdd(char c);
uint8_t Cal_Run(void);

void Cal_Exit(void);

#endif
