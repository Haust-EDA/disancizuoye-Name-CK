#include "stm32f10x.h"
#include "GVar.h"
#include "Control.h"
#include "OLED.h"

void UI_Mark(uint8_t Choice_Index)
{
	if (Choice_Index == 0)
		return;
	// 清空选项显示
	for (uint8_t i = 1; i < 5; i++)
	{
		OLED_ShowChar(i, 1, ' ');
	}
	// 标记显示
	OLED_ShowChar(Choice_Index, 1, '*');
}
/**
 * @brief 主菜单
 */
void UI_MenuMain(uint8_t *Choice_Index)
{
	if (*Choice_Index > 3)
	{
		*Choice_Index = 1;
	}
	OLED_ShowString(1, 2, "LED");
	OLED_ShowString(2, 2, "Calculator");
	OLED_ShowString(3, 2, "Chronograph");
	UI_Mark(*Choice_Index);
}
/**
 * @brief LED功能选择菜单
 */
void UI_MenuLED(uint8_t *Choice_Index)
{
	if (*Choice_Index > 3)
	{
		*Choice_Index = 1;
	}
	OLED_ShowString(1, 2, "LightWater ");
	OLED_ShowString(2, 2, "Breathing_LW");
	OLED_ShowString(3, 2, "Transform");
	UI_Mark(*Choice_Index);
}


void UI_Show(void)
{
	switch (G_UIState)
	{
	case 1:
		UI_MenuMain(&G_Choice_Index);
		break;
	case 2:
		UI_MenuLED(&G_Choice_Index);
		break;
	default:
		UI_MenuMain(&G_Choice_Index);
		break;
	}
}
/**
 * @brief 界面跳转
 * @param dir	:跳转方向；0上一级，1下一级
 */
void UI_Trun(uint8_t dir)
{
	if (dir == 0) // 上一级
	{
		switch (G_UIState)
		{
		case UIT_MenuMain:
			G_UIState = UIT_MenuMain;
			break;
		case UIT_MenuLED:
			G_UIState = UIT_MenuMain;
			break;
		default:
			G_UIState = UIT_MenuMain;
			break;
		}
		G_Choice_Index = 1;
	}
	if (dir == 1) // 下一级
	{
		// 双层判断，外层判断当前界面，内层判断当前选择
		switch (G_UIState) // 判断当前的界面
		{
		case UIT_MenuMain:			// 当前在主界面
			switch (G_Choice_Index) // 判断当前的选择
			{
			case 1:
				G_UIState = UIT_MenuLED;
				break;
			default:
				G_UIState = UIT_MenuMain;
				break;
			}
		case UIT_MenuLED:			// 当前在主界面
			switch (G_Choice_Index) // 判断当前的选择
			{
			case 1:
				G_UIState = UIT_MenuLED;
				break;
			default:
				G_UIState = UIT_MenuMain;
				break;
			}
		default:
			G_UIState = UIT_MenuMain;
			break;
		}
	}
}
