#include "stm32f10x.h"
#include "LCD.h"
#include "Control.h"
#include "Stopwatch.h"

uint8_t C_ShowIndex = 0; // 当前显示页面编号

extern uint32_t TimeTick;
extern uint8_t KeyNum;
extern uint8_t KeyState;
/*  主菜单  */
uint8_t C_MenuShow_Index = 1;
/**
 * @brief 显示主菜单
 */
void C_MenuShow(void) // C_ShowIndex->0
{
	// LCD_Fill(0,0,160,128,WHITE);
	LCD_ShowString(5, 5 + 30 * 0, " LED", BLACK, WHITE, 24, 0);
	LCD_ShowString(5, 5 + 30 * 1, " Calculator", BLACK, WHITE, 24, 0);
	LCD_ShowString(5, 5 + 30 * 2, " Stopwatch", BLACK, WHITE, 24, 0);
	// LCD_Fill(0, 0, 16, 128, WHITE);
	LCD_ShowChar(5, 5 + 30 * C_MenuShow_Index, '>', BLACK, WHITE, 24, 0);
}
void C_MenuKey(void)
{
	switch (KeyNum)
	{
	case 2: // 上
		if (C_MenuShow_Index != 0)
		{
			C_MenuShow_Index--;
		}
		C_MenuShow();
		break;
	case 10: // 下
		if (C_MenuShow_Index != 2)
		{
			C_MenuShow_Index++;
		}
		C_MenuShow();
		
		break;
	case 15: // 进入
		switch (C_MenuShow_Index)
		{
		case 1:
			C_ShowIndex = 2;
			LCD_Fill(0, 0, 160, 128, WHITE);
			C_CalShow();
			break;
		case 2:
			C_ShowIndex = 3;
			LCD_Fill(0, 0, 160, 128, WHITE);
			C_SWShow();
			
			break;
		default:
			break;
		}
	default:
		break;
	}
}
/*	计算器	*/
uint8_t C_Cal_State = 0; // 计算器状态

char C_Cal_Str[13*2] = ""; // 显示字符串
uint8_t C_Cal_Str_Index = 0; // 显示字符串索引

void C_CalShow(void) // C_ShowIndex->1
{
	C_Cal_State=1;
	LCD_ShowString(0, 0, "Calculator", GREEN, WHITE, 12, 0); // 标题
	LCD_ShowString(0, 10, "<", BLACK, WHITE, 24, 0);
}
//void C_CalShowRe(void)
//{
//	uint8_t Show_Index=0;
//	char str[14];
//	while(C_Cal_Str_Index!=0 && Show_Index<=C_Cal_Str_Index)
//	{
//		while(Show_Index<C_Cal_Str_Index && Show_Index<13)
//		{
//			str[Show_Index]=C_Cal_Str[Show_Index];
//		}
//	}
//}
void C_CalKey(void)
{
	if(C_Cal_State==1)//输入状态
	{
		switch (KeyNum)
		{
		case 1:
			
			break;
		
		default:
			break;
		}
	}
}


/*  秒表    */
uint8_t C_SWShow_TIndex = 0; // 记录显示的序号
/**
 * @brief 显示主计时
 */
void C_SWShowTime0(void)
{
	SW_TimeRe(); // 刷新时间
	// 计算
	uint8_t min, sec, msec;
	SW_Timef(SW_Time0, &min, &sec, &msec);
	LCD_ShowString(15 + 16 * 0, 10, Strf("%02d:", min), BLACK, WHITE, 32, 0);		   // 显示分
	LCD_ShowString(15 + 16 * 3, 10, Strf("%02d.%02d", sec, msec), BLUE, WHITE, 32, 0); // 显示秒

	/*
	// 显示分
	if(min<10)
	{
		LCD_ShowChar(15 + 16 * 0, 10, '0', BLACK, WHITE, 32, 0);
		LCD_ShowIntNum(15 + 16 * 1, 10, min, 1, BLACK, WHITE, 32);
	}
	else
	{
		LCD_ShowIntNum(15 + 16 * 0, 10, min, 2, BLACK, WHITE, 32);
	}
	LCD_ShowChar(15 + 16 * 2, 10, ':', BLACK, WHITE, 32, 0);

	// 显示秒
	if(sec<10)
	{
		LCD_ShowChar(15 + 16 * 3, 10, '0', BLUE, WHITE, 32, 0);
		LCD_ShowIntNum(15 + 16 * 4, 10, sec, 1, BLUE, WHITE, 32);
	}
	else
	{
		LCD_ShowIntNum(15 + 16 * 3, 10, sec, 2, BLUE, WHITE, 32);
	}
	// 显示毫秒
	LCD_ShowChar(15 + 16 * 5, 10, '.', BLUE, WHITE, 32, 0);
	if(msec<10)
	{
		LCD_ShowChar(15 + 16 * 6, 10, '0', BLUE, WHITE, 32, 0);
		LCD_ShowIntNum(15 + 16 * 7, 10, sec, 1, BLUE, WHITE, 32);
	}
	else
	{
		LCD_ShowIntNum(15 + 16 * 6, 10, msec, 2, BLUE, WHITE, 32);
	}
	*/
}
void C_SWShowTimes(void)
{
	uint8_t num = C_SWShow_TIndex;
	if (num == 0)
	{
		LCD_Fill(0, 40, 160, 128, WHITE);
	}
	for (num = C_SWShow_TIndex; (num > C_SWShow_TIndex - 6) && num > 0; num--)
	{
		uint8_t min, sec, msec;
		uint8_t l_min, l_sec, l_msec;
		uint8_t show_y = 40 + 14 * (C_SWShow_TIndex - num);

		SW_Timef(SW_Times[num - 1], &min, &sec, &msec);
		LCD_ShowString(1, show_y, Strf("%02d", num), RED, WHITE, 16, 0);						 // 显示序号
		LCD_ShowString(22, show_y, Strf("%02d:%02d.%02d", min, sec, msec), BLACK, WHITE, 16, 0); // 显示记录的时间

		if (num == 1) // 显示时间差
		{
			LCD_ShowString(95, show_y, "+00:00.0", BLACK, WHITE, 16, 0);
		}
		else
		{
			SW_Timef(SW_Times[num - 1] - SW_Times[num - 2], &l_min, &l_sec, &l_msec);
			LCD_ShowString(95, show_y, Strf("+%02d:%02d.%01d", l_min, l_sec, l_msec), BLACK, WHITE, 16, 0);
		}
	}
}
/**
 * @brief 显示Stopwatch
 */
void C_SWShow(void) // C_ShowIndex->3
{
	LCD_ShowString(0, 0, "Stopwatch", GREEN, WHITE, 12, 0); // 标题
	C_SWShowTime0();
	C_SWShowTimes();
}
void C_SWKey(void)
{
	switch (KeyNum)
	{
	case 4: // 开始(未开始时)|暂停/继续(开始后)

		if (SW_Flag == 0) // 未计时
		{
			SW_Start();
		}
		else if (SW_Flag == 1 || 2) // 计时中|暂停中
		{
			SW_Pause();
		}
		break;

	case 8: // 记录(计时时)|复位(暂停时)

		if (SW_Flag == 1) // 计时中
		{
			SW_Record();
			C_SWShow_TIndex = SW_TimesIndex; // 记录序号
			C_SWShowTimes();				 // 刷新记录时间的显示
		}
		else if (SW_Flag == 2) // 暂停中
		{

			SW_Reset();
			C_SWShow_TIndex = 0;
			C_SWShowTime0(); // 刷新主时间显示
			C_SWShowTimes(); // 刷新记录时间的显示
		}
		break;

	case 2: // 记录上移
		if (C_SWShow_TIndex < SW_TimesIndex)
		{
			C_SWShow_TIndex++;
		}
		C_SWShowTimes(); // 刷新记录时间的显示
		break;

	case 10: // 记录下移
		if (C_SWShow_TIndex > 6)
		{
			C_SWShow_TIndex--;
		}
		C_SWShowTimes(); // 刷新记录时间的显示
		break;

	case 14: // 退出

		if (KeyState == 2) // 长按
		{
			C_ShowIndex = 0;
			LCD_Fill(0, 0, 160, 128, WHITE);
			C_MenuShow();
		}
		break;

	default:
		break;
	}
}
/**
 * @brief 刷新屏幕内容
 */
void C_ShowRe(void)
{
	if (C_ShowIndex == 3 && SW_Flag == 1)
	{
		C_SWShowTime0();
	}
}
/*	通用	*/
uint8_t *Strf(char *format, ...)
{
	static char String[255];
	// 格式化字符
	va_list arg;
	va_start(arg, format);
	vsprintf(String, format, arg);
	va_end(arg);
	// 返回字符串指针
	return (uint8_t *)String;
}
