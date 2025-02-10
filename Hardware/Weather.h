/**
 * ջ�Ŀռ䲻��,������
 */
#ifndef __WEATHER_H_
#define __WEATHER_H_
#include "stm32f10x.h" // Device header

// ���� cast ����������̶�Ϊ 4 ����
#define CAST_COUNT 4

// ���� cast �ṹ�壨���ֶγ��ȿɸ���ʵ�����������
typedef struct
{
    char date[11];                // "2025-02-08" � 10+'\0'
    char week[3];                 // "6" �� "10"
    char dayweather[3 * 8 + 1];   // �ռ�����
    char nightweather[3 * 8 + 1]; // ҹ������
    char daytemp[8];              // �ռ��¶�
    char nighttemp[8];            // ҹ���¶�
    char daywind[3 * 4 + 1];      // �ռ����
    char nightwind[3 * 4 + 1];    // ҹ�����
    char daypower[5];             // �ռ����
    char nightpower[5];           // ҹ�����
    char daytemp_float[8];        // �ռ��¶ȸ�����
    char nighttemp_float[8];      // ҹ���¶ȸ�����
} cast_t;

extern uint8_t Esp_isConnect;
extern uint8_t RxData[2048]; // �������ݻ�����

extern uint16_t RxIndex;                // �������ݵ��ֽ���
extern cast_t WeatherCasts[CAST_COUNT]; // �洢����Ԥ�����ݵ�����
extern uint8_t WeatherIsGet;            // ���������Ƿ��ȡ�ɹ�

void ESP_Init(void);
void ESP_Connect(void);

uint8_t Weather_Parse(void);
#endif
