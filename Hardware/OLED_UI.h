/**
 * 负责界面的显示
 * 使用OLED.h中的OLED_Init();初始化
 */
#ifndef __OLED_UI_H
#define __OLED_UI_H

#include "OLED.h"


void UI_Mark(uint8_t Choice_Index);//标记某行被选中
void UI_MenuMain(uint8_t* Choice_Index);//主界面
void UI_MenuLED(uint8_t* Choice_Index);//LED功能选择界面
void UI_Show(void);//根据G_UI_State更新界面显示
void UI_Trun(uint8_t dir);//界面跳转
#endif

