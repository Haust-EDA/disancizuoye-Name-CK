#include "stm32f10x.h" // Device header
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "Weather.h"
#include "Delay.h"
#include "jsmn.h"

// USART 1
#define USART1_StopBits USART_StopBits_1	  // 停止位 1
#define USART1_WordLength USART_WordLength_8b // 数据位 8
#define USART1_Parity USART_Parity_No		  // 校验位 NONE
#define USART1_BaudRate 115200				  // 波特率

/* 全局变量 */
uint8_t RxData[2048]; // 接收数据缓存区

uint16_t RxIndex = 0; // 接收数据的字节数
uint8_t Esp_isConnect = 1;
uint8_t Esp_isRx = 0;
cast_t WeatherCasts[CAST_COUNT];

// /**
//  * @brief 格式化字符串
//  */
// uint8_t *Strf(char *format, ...)
// {
// 	static char String[255];
// 	// 格式化字符
// 	va_list arg;
// 	va_start(arg, format);
// 	vsprintf(String, format, arg);
// 	va_end(arg);
// 	// 返回字符串指针
// 	return (uint8_t *)String;
// }


/**
 * @brief   发送数组(类型 uint8_t)
 * @param   Arr		待发送数组的指针
 * @param   Length	数组长度
 * @retval  无
 */
void SerialWrite(uint8_t *Arr, uint16_t Length)
{
	uint16_t i = 0;
	for (i = 0; i < Length && Arr[i] != '\0'; i++)
	{
		USART_SendData(USART1, Arr[i]);
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
			; // USART_FLAG_TXE	发送数据寄存器空标志位
	}
}
/**
 * @brief   获得接收的数组
 * @param   Arr 	存储待接收数组的指针
 * @param   Length 	数组长度
 * @retval  (uint8_t) 0失败;1成功;2成功但提取的长度小于缓存区
 * @warning 当缓存区的数据位数超出需要的时候，超出Length的字节会被忽略
 * @todo    解决warning
 */
uint8_t SerialRead(uint8_t *Arr, uint8_t Length)
{
	if (RxIndex >= Length)
	{
		// 转移接受到的数据至Arr
		for (uint8_t i = 0; i < Length; i++)
		{
			Arr[i] = RxData[i];
		}

		// 判断请求的数据长度与缓存的数据长度是否相等
		if (RxIndex == Length)
		{
			RxIndex = 0;
			return 1;
		}
		else if (RxIndex > Length)
		{
			RxIndex = 0;
			return 2;
		}
	}
	return 0;
}
/**
 * @brief 串口初始化
 */
void USART1_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitTypeDef GPIO_Itd;
	GPIO_Itd.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Itd.GPIO_Pin = GPIO_Pin_6; // TX
	GPIO_Itd.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_Itd);

	GPIO_Itd.GPIO_Pin = GPIO_Pin_7; // RX
	GPIO_Itd.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_Itd);
	GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);

	USART_InitTypeDef USART_Itd;
	USART_StructInit(&USART_Itd);
	USART_Itd.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	/****** 以下数据在 Serial.h 文件中修改 ******/
	USART_Itd.USART_StopBits = USART1_StopBits;		// 停止位
	USART_Itd.USART_WordLength = USART1_WordLength; // 数据位
	USART_Itd.USART_Parity = USART1_Parity;			// 校验位
	USART_Itd.USART_BaudRate = USART1_BaudRate;		// 波特率
	/*********************************************/
	USART_Init(USART1, &USART_Itd);

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef NVIC_Itd;
	NVIC_Itd.NVIC_IRQChannel = USART1_IRQn;
	NVIC_Itd.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Itd.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_Itd.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_Itd);

	USART_Cmd(USART1, ENABLE);
}
void USART1_IRQHandler(void)
{
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
		if (Esp_isRx == 1)
		{
			RxData[RxIndex++] = USART_ReceiveData(USART1);
			RxData[RxIndex] = '\0';
		}

		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
}
/************************************************/
void ESP_Init(void)
{
	USART1_Init();
}

void ESP_Connect(void)
{
	SerialWrite((uint8_t *)"AT+RST\r\n", 8);
	Delay_s(6);
	RxIndex = 0;
	SerialWrite((uint8_t *)"ATE0\r\n", 6);
	Delay_ms(200);
	RxIndex = 0;
	SerialWrite((uint8_t *)"AT+CIPSTART=\"TCP\",\"restapi.amap.com\",80\r\n", 42);
	Delay_s(2);
	SerialWrite((uint8_t *)"AT+CIPMODE=1\r\n", 14);
	Delay_ms(300);
	SerialWrite((uint8_t *)"AT+CIPSEND\r\n", 12);
	Delay_ms(300);
	RxIndex = 0;
	Esp_isRx = 1;
	SerialWrite((uint8_t *)"GET http://restapi.amap.com/v3/weather/weatherInfo?key=9e78f303567d7abf2520e4ff9b930acb&city=411402&extensions=all\n", 117);
	Delay_s(1);
}

/***************************************/

/*
 * 辅助函数：判断 token 表示的字符串是否等于 s
 * 返回 0 表示相等，非 0 表示不相等
 */
static int jsoneq(const char *json, jsmntok_t *tok, const char *s)
{
	if (tok->type == JSMN_STRING &&
		(int)strlen(s) == tok->end - tok->start &&
		strncmp(json + tok->start, s, tok->end - tok->start) == 0)
	{
		return 0;
	}
	return -1;
}

/*
 * 函数：parse_casts
 * 作用：使用 jsmn 解析传入的 JSON 字符串，从中提取出 4 个 cast 对象，并将各字段复制到 casts 数组中。
 * 参数：
 *    json  - 待解析的 JSON 字符串
 *    casts - 存储解析结果的 cast 数组（大小必须不小于 CAST_COUNT）
 * 返回值：
 *    0   - 成功
 *    非0 - 不同数值代表不同出错步骤（例如：1 代表解析错误，2 代表根节点不是对象，…）
 */
uint8_t parse_casts(const char *json, cast_t casts[CAST_COUNT])
{
	jsmn_parser parser;
	jsmntok_t tokens[128];
	int token_count;
	int i, pos;

	jsmn_init(&parser);
	token_count = jsmn_parse(&parser, json, strlen(json), tokens, sizeof(tokens) / sizeof(tokens[0]));
	if (token_count < 0)
	{
		return 1; // 解析错误
	}
	if (token_count < 1 || tokens[0].type != JSMN_OBJECT)
	{
		return 2; // 根节点不是对象
	}

	/* 查找根对象中的 "forecasts" 键 */
	pos = 1;					   // 从第 1 个 token 开始（token[0] 为整个对象）
	int num_keys = tokens[0].size; // 根对象中键值对的数量
	int forecasts_index = -1;
	for (i = 0; i < num_keys; i++)
	{
		if (jsoneq(json, &tokens[pos], "forecasts") == 0)
		{
			forecasts_index = pos + 1; // forecasts 的值 token 索引
			break;
		}
		pos += 2; // 每个键值对占 2 个 token
	}
	if (forecasts_index == -1)
	{
		return 3; // 没有找到 "forecasts" 键
	}
	if (tokens[forecasts_index].type != JSMN_ARRAY)
	{
		return 4; // "forecasts" 对应的值不是数组
	}
	if (tokens[forecasts_index].size < 1)
	{
		return 5; // "forecasts" 数组为空
	}

	/* 取 forecasts 数组中第 1 个元素（forecast 对象） */
	int forecast_index = forecasts_index + 1;
	if (tokens[forecast_index].type != JSMN_OBJECT)
	{
		return 6; // forecast 元素不是对象
	}

	/* 在 forecast 对象中查找 "casts" 键 */
	int forecast_obj_size = tokens[forecast_index].size;
	pos = forecast_index + 1;
	int casts_index = -1;
	for (i = 0; i < forecast_obj_size; i++)
	{
		if (jsoneq(json, &tokens[pos], "casts") == 0)
		{
			casts_index = pos + 1; // casts 数组的 token 索引
			break;
		}
		pos += 2;
	}
	if (casts_index == -1)
	{
		return 7; // 没有找到 "casts" 键
	}
	if (tokens[casts_index].type != JSMN_ARRAY)
	{
		return 8; // "casts" 对应的值不是数组
	}
	if (tokens[casts_index].size != CAST_COUNT)
	{
		return 9; // "casts" 数组内的对象数量不是 4 个
	}

	/* 解析 casts 数组内的每个 cast 对象 */
	pos = casts_index + 1; // 指向第一个 cast 对象
	for (i = 0; i < CAST_COUNT; i++)
	{
		if (tokens[pos].type != JSMN_OBJECT)
		{
			return 10; // 期望 cast 对象
		}
		int cast_obj_size = tokens[pos].size;
		/* 每个对象包含 cast_obj_size 个键值对，接下来连续有 2*cast_obj_size 个 token */
		int obj_end = pos + 1 + cast_obj_size * 2;
		int j;
		for (j = pos + 1; j < obj_end; j += 2)
		{
			jsmntok_t *key = &tokens[j];
			jsmntok_t *val = &tokens[j + 1];
			int len = val->end - val->start;
			if (jsoneq(json, key, "date") == 0)
			{
				if (len >= (int)sizeof(casts[i].date))
					len = sizeof(casts[i].date) - 1;
				strncpy(casts[i].date, json + val->start, len);
				casts[i].date[len] = '\0';
			}
			else if (jsoneq(json, key, "week") == 0)
			{
				if (len >= (int)sizeof(casts[i].week))
					len = sizeof(casts[i].week) - 1;
				strncpy(casts[i].week, json + val->start, len);
				casts[i].week[len] = '\0';
			}
			else if (jsoneq(json, key, "dayweather") == 0)
			{
				if (len >= (int)sizeof(casts[i].dayweather))
					len = sizeof(casts[i].dayweather) - 1;
				strncpy(casts[i].dayweather, json + val->start, len);
				casts[i].dayweather[len] = '\0';
			}
			else if (jsoneq(json, key, "nightweather") == 0)
			{
				if (len >= (int)sizeof(casts[i].nightweather))
					len = sizeof(casts[i].nightweather) - 1;
				strncpy(casts[i].nightweather, json + val->start, len);
				casts[i].nightweather[len] = '\0';
			}
			else if (jsoneq(json, key, "daytemp") == 0)
			{
				if (len >= (int)sizeof(casts[i].daytemp))
					len = sizeof(casts[i].daytemp) - 1;
				strncpy(casts[i].daytemp, json + val->start, len);
				casts[i].daytemp[len] = '\0';
			}
			else if (jsoneq(json, key, "nighttemp") == 0)
			{
				if (len >= (int)sizeof(casts[i].nighttemp))
					len = sizeof(casts[i].nighttemp) - 1;
				strncpy(casts[i].nighttemp, json + val->start, len);
				casts[i].nighttemp[len] = '\0';
			}
			else if (jsoneq(json, key, "daywind") == 0)
			{
				if (len >= (int)sizeof(casts[i].daywind))
					len = sizeof(casts[i].daywind) - 1;
				strncpy(casts[i].daywind, json + val->start, len);
				casts[i].daywind[len] = '\0';
			}
			else if (jsoneq(json, key, "nightwind") == 0)
			{
				if (len >= (int)sizeof(casts[i].nightwind))
					len = sizeof(casts[i].nightwind) - 1;
				strncpy(casts[i].nightwind, json + val->start, len);
				casts[i].nightwind[len] = '\0';
			}
			else if (jsoneq(json, key, "daypower") == 0)
			{
				if (len >= (int)sizeof(casts[i].daypower))
					len = sizeof(casts[i].daypower) - 1;
				strncpy(casts[i].daypower, json + val->start, len);
				casts[i].daypower[len] = '\0';
			}
			else if (jsoneq(json, key, "nightpower") == 0)
			{
				if (len >= (int)sizeof(casts[i].nightpower))
					len = sizeof(casts[i].nightpower) - 1;
				strncpy(casts[i].nightpower, json + val->start, len);
				casts[i].nightpower[len] = '\0';
			}
			else if (jsoneq(json, key, "daytemp_float") == 0)
			{
				if (len >= (int)sizeof(casts[i].daytemp_float))
					len = sizeof(casts[i].daytemp_float) - 1;
				strncpy(casts[i].daytemp_float, json + val->start, len);
				casts[i].daytemp_float[len] = '\0';
			}
			else if (jsoneq(json, key, "nighttemp_float") == 0)
			{
				if (len >= (int)sizeof(casts[i].nighttemp_float))
					len = sizeof(casts[i].nighttemp_float) - 1;
				strncpy(casts[i].nighttemp_float, json + val->start, len);
				casts[i].nighttemp_float[len] = '\0';
			}
		}
		pos = obj_end; // 指向下一个 cast 对象
	}

	return 0; // 成功
}
uint8_t Weather_Parse(void)
{
	for (uint8_t i = 0; i < CAST_COUNT; i++)
	{
		memset(&WeatherCasts[i], 0, sizeof(cast_t));
	}
	uint8_t ret = parse_casts((char *)RxData, WeatherCasts);
	return ret;
}
