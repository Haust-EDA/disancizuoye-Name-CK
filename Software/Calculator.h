#ifndef __CALCULATOR_H_
#define __CALCULATOR_H_
#include "stdint.h"

#define STRIN_MAX_LENGTH 10 * 4

extern char CalStrIn[STRIN_MAX_LENGTH]; // 输入的字符串
extern uint8_t CalStrIn_Index;          // 输入字符串索引

extern double CalResult;

uint8_t Cal_StrInAdd(char c);
uint8_t Cal_Run(void);

void Cal_Exit(void);

#ifndef NULL
#define NULL (void *)0
#endif

#endif
