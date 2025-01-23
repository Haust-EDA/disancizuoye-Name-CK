#include "Calculator.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char CalStrIn[STRIN_MAX_LENGTH] = ""; // 输入的字符串
uint8_t CalStrIn_Index = 0;           // 输入字符串索引
double CalResult = 0;                 // 计算结果

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
 * @brief 重置计数器
 * @return 无
 */
void Cal_Exit(void)
{
	CalStrIn_Index = 0;
	CalStrIn[0] = '\0';
}

// 判断字符是否为运算符
int is_operator(char c)
{
	return (c == '+' || c == '-' || c == '*' || c == '/');
}

// 计算两个数字的运算结果
double compute(double a, double b, char operator, uint8_t * valid)
{
	switch (operator)
	{
	case '+':
		return a + b;
	case '-':
		return a - b;
	case '*':
		return a * b;
	case '/':
		if (b == 0)
		{
			*valid = 0; // 设置valid为0表示非法
			return 0.0; // 除以零，返回错误值
		}
		return a / b;
	default:
		return 0.0;
	}
}

// 检查并计算表达式
double evaluate_expression(const char *expr, uint8_t *valid)
{
	double result = 0.0;
	double current_number = 0.0;
	char operator= '+';
	int i = 0;
	*valid = 1; // 默认合法

	// 验证表达式最后是否为'='
	int len = strlen(expr);
	if (expr[len - 1] != '=')
	{
		*valid = 0;
		return 0.0;
	}

	// 执行表达式的计算，去掉最后的'='
	while (expr[i] != '\0' && expr[i] != '=')
	{
		if (isdigit(expr[i]) || expr[i] == '.')
		{
			// 提取当前数字（支持浮点数）
			current_number = strtod(&expr[i], NULL);
			// 移动指针，跳过数字部分
			while (isdigit(expr[i]) || expr[i] == '.')
			{
				i++;
			}
			// 根据上一个运算符计算
			result = compute(result, current_number, operator, valid);

			if (*valid == 0)
			{
				return 0.0; // 如果遇到非法操作（如除以零），提前返回
			}
		}
		else if (is_operator(expr[i]))
		{
			// 当前字符是运算符
			operator= expr[i];
			i++;
		}
		else if (isspace(expr[i]))
		{
			// 跳过空格
			i++;
		}
		else
		{
			// 非法字符
			*valid = 0;
			return 0.0;
		}
	}

	return result;
}

uint8_t Cal_Run(void)
{
	uint8_t CalValid = 1;

	CalResult = evaluate_expression(CalStrIn, &CalValid);
	if (CalValid)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}
