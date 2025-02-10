/**
 * 栈的空间不足,需增加
 */
#ifndef __WEATHER_H_
#define __WEATHER_H_
#include "stm32f10x.h" // Device header

// 定义 cast 数组个数（固定为 4 个）
#define CAST_COUNT 4

// 定义 cast 结构体（各字段长度可根据实际情况调整）
typedef struct
{
    char date[11];                // "2025-02-08" 最长 10+'\0'
    char week[3];                 // "6" 或 "10"
    char dayweather[3 * 8 + 1];   // 日间天气
    char nightweather[3 * 8 + 1]; // 夜间天气
    char daytemp[8];              // 日间温度
    char nighttemp[8];            // 夜间温度
    char daywind[3 * 4 + 1];      // 日间风向
    char nightwind[3 * 4 + 1];    // 夜间风向
    char daypower[5];             // 日间风力
    char nightpower[5];           // 夜间风力
    char daytemp_float[8];        // 日间温度浮点数
    char nighttemp_float[8];      // 夜间温度浮点数
} cast_t;

extern uint8_t Esp_isConnect;
extern uint8_t RxData[2048]; // 接收数据缓存区

extern uint16_t RxIndex;                // 接收数据的字节数
extern cast_t WeatherCasts[CAST_COUNT]; // 存储天气预报数据的数组
extern uint8_t WeatherIsGet;            // 天气数据是否获取成功

void ESP_Init(void);
void ESP_Connect(void);

uint8_t Weather_Parse(void);
#endif
