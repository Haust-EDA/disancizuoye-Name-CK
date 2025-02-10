#include "stm32f10x.h" // Device header
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "Weather.h"
#include "Delay.h"
#include "jsmn.h"

// USART 1
#define USART1_StopBits USART_StopBits_1	  // ֹͣλ 1
#define USART1_WordLength USART_WordLength_8b // ����λ 8
#define USART1_Parity USART_Parity_No		  // У��λ NONE
#define USART1_BaudRate 115200				  // ������

/* ȫ�ֱ��� */
uint8_t RxData[2048]; // �������ݻ�����

uint16_t RxIndex = 0; // �������ݵ��ֽ���
uint8_t Esp_isConnect = 1;
uint8_t Esp_isRx = 0;
cast_t WeatherCasts[CAST_COUNT];

// /**
//  * @brief ��ʽ���ַ���
//  */
// uint8_t *Strf(char *format, ...)
// {
// 	static char String[255];
// 	// ��ʽ���ַ�
// 	va_list arg;
// 	va_start(arg, format);
// 	vsprintf(String, format, arg);
// 	va_end(arg);
// 	// �����ַ���ָ��
// 	return (uint8_t *)String;
// }


/**
 * @brief   ��������(���� uint8_t)
 * @param   Arr		�����������ָ��
 * @param   Length	���鳤��
 * @retval  ��
 */
void SerialWrite(uint8_t *Arr, uint16_t Length)
{
	uint16_t i = 0;
	for (i = 0; i < Length && Arr[i] != '\0'; i++)
	{
		USART_SendData(USART1, Arr[i]);
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
			; // USART_FLAG_TXE	�������ݼĴ����ձ�־λ
	}
}
/**
 * @brief   ��ý��յ�����
 * @param   Arr 	�洢�����������ָ��
 * @param   Length 	���鳤��
 * @retval  (uint8_t) 0ʧ��;1�ɹ�;2�ɹ�����ȡ�ĳ���С�ڻ�����
 * @warning ��������������λ��������Ҫ��ʱ�򣬳���Length���ֽڻᱻ����
 * @todo    ���warning
 */
uint8_t SerialRead(uint8_t *Arr, uint8_t Length)
{
	if (RxIndex >= Length)
	{
		// ת�ƽ��ܵ���������Arr
		for (uint8_t i = 0; i < Length; i++)
		{
			Arr[i] = RxData[i];
		}

		// �ж���������ݳ����뻺������ݳ����Ƿ����
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
 * @brief ���ڳ�ʼ��
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
	/****** ���������� Serial.h �ļ����޸� ******/
	USART_Itd.USART_StopBits = USART1_StopBits;		// ֹͣλ
	USART_Itd.USART_WordLength = USART1_WordLength; // ����λ
	USART_Itd.USART_Parity = USART1_Parity;			// У��λ
	USART_Itd.USART_BaudRate = USART1_BaudRate;		// ������
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
 * �����������ж� token ��ʾ���ַ����Ƿ���� s
 * ���� 0 ��ʾ��ȣ��� 0 ��ʾ�����
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
 * ������parse_casts
 * ���ã�ʹ�� jsmn ��������� JSON �ַ�����������ȡ�� 4 �� cast ���󣬲������ֶθ��Ƶ� casts �����С�
 * ������
 *    json  - �������� JSON �ַ���
 *    casts - �洢��������� cast ���飨��С���벻С�� CAST_COUNT��
 * ����ֵ��
 *    0   - �ɹ�
 *    ��0 - ��ͬ��ֵ����ͬ�����裨���磺1 �����������2 ������ڵ㲻�Ƕ��󣬡���
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
		return 1; // ��������
	}
	if (token_count < 1 || tokens[0].type != JSMN_OBJECT)
	{
		return 2; // ���ڵ㲻�Ƕ���
	}

	/* ���Ҹ������е� "forecasts" �� */
	pos = 1;					   // �ӵ� 1 �� token ��ʼ��token[0] Ϊ��������
	int num_keys = tokens[0].size; // �������м�ֵ�Ե�����
	int forecasts_index = -1;
	for (i = 0; i < num_keys; i++)
	{
		if (jsoneq(json, &tokens[pos], "forecasts") == 0)
		{
			forecasts_index = pos + 1; // forecasts ��ֵ token ����
			break;
		}
		pos += 2; // ÿ����ֵ��ռ 2 �� token
	}
	if (forecasts_index == -1)
	{
		return 3; // û���ҵ� "forecasts" ��
	}
	if (tokens[forecasts_index].type != JSMN_ARRAY)
	{
		return 4; // "forecasts" ��Ӧ��ֵ��������
	}
	if (tokens[forecasts_index].size < 1)
	{
		return 5; // "forecasts" ����Ϊ��
	}

	/* ȡ forecasts �����е� 1 ��Ԫ�أ�forecast ���� */
	int forecast_index = forecasts_index + 1;
	if (tokens[forecast_index].type != JSMN_OBJECT)
	{
		return 6; // forecast Ԫ�ز��Ƕ���
	}

	/* �� forecast �����в��� "casts" �� */
	int forecast_obj_size = tokens[forecast_index].size;
	pos = forecast_index + 1;
	int casts_index = -1;
	for (i = 0; i < forecast_obj_size; i++)
	{
		if (jsoneq(json, &tokens[pos], "casts") == 0)
		{
			casts_index = pos + 1; // casts ����� token ����
			break;
		}
		pos += 2;
	}
	if (casts_index == -1)
	{
		return 7; // û���ҵ� "casts" ��
	}
	if (tokens[casts_index].type != JSMN_ARRAY)
	{
		return 8; // "casts" ��Ӧ��ֵ��������
	}
	if (tokens[casts_index].size != CAST_COUNT)
	{
		return 9; // "casts" �����ڵĶ����������� 4 ��
	}

	/* ���� casts �����ڵ�ÿ�� cast ���� */
	pos = casts_index + 1; // ָ���һ�� cast ����
	for (i = 0; i < CAST_COUNT; i++)
	{
		if (tokens[pos].type != JSMN_OBJECT)
		{
			return 10; // ���� cast ����
		}
		int cast_obj_size = tokens[pos].size;
		/* ÿ��������� cast_obj_size ����ֵ�ԣ������������� 2*cast_obj_size �� token */
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
		pos = obj_end; // ָ����һ�� cast ����
	}

	return 0; // �ɹ�
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
