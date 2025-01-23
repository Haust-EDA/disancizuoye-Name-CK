#include "stm32f10x.h"
#include "LCD.h"
#include "Control.h"
#include "Delay.h"
#include "Stopwatch.h"
#include "Calculator.h"

uint8_t C_ShowIndex = 0; // 当前显示页面编号

extern uint32_t TimeTick;
extern uint8_t KeyNum;
extern uint8_t KeyState;

/****************************************************************/
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

/****************************************************************/
/*	计算器	*/
#define _C_CAL_SHOW_ROW 4  // 行数
#define _C_CAL_SHOW_COL 10 // 列数
uint8_t C_Cal_State = 0;   // 计算器状态

void C_CalShow(void) // C_ShowIndex->1
{
	C_Cal_State = 1;
	LCD_ShowString(0, 0, "Calculator", GREEN, WHITE, 12, 0); // 标题
	LCD_ShowString(0, 10, "<", BLACK, WHITE, 24, 0);
}
/**
 * @brief 显示内容刷新
 */
void C_CalShowRe(void)
{
	if (C_Cal_State == 1)
	{
		uint8_t showRow = ((CalStrIn_Index - 1) / _C_CAL_SHOW_COL) % _C_CAL_SHOW_ROW;
		uint8_t showCol = (CalStrIn_Index - 1) % _C_CAL_SHOW_COL;
		uint16_t show_x = 0 + 16 * showCol, show_y = 10 + 22 * showRow;
		char c = CalStrIn[CalStrIn_Index - 1];
		LCD_ShowChar(show_x, show_y, c, BLACK, WHITE, 24, 0);

		showRow = (CalStrIn_Index / _C_CAL_SHOW_COL) % _C_CAL_SHOW_ROW;
		showCol = CalStrIn_Index % _C_CAL_SHOW_COL;
		show_x = 0 + 16 * showCol, show_y = 10 + 22 * showRow;
		if (CalStrIn_Index == STRIN_MAX_LENGTH - 1)
		{
			c = '|';
		}
		else
		{
			c = '<';
		}
		LCD_ShowChar(show_x, show_y, c, BLACK, WHITE, 24, 0);
	}
}
/**
 * @brief 显示结果
 */
void C_Cal_ShowRes(void)
{
	if (Cal_Run() == 0)
	{

		long long int temp = (CalResult > 0) ? (long long int)CalResult : (long long int)(-CalResult);
		uint8_t Num = 0; // 结果的位数
		while (temp > 0)
		{
			temp /= 10;
			Num++;
		}
		Num += 3;

		if (Num > 19)
		{
			LCD_ShowString(0, 10 + 22 * 4, "ERR:TOO LONG", RED, WHITE, 24, 0);
		}
		else if (Num > 10)
		{
			LCD_ShowString(0, 10 + 22 * 4, Strf("%.2f", CalResult), BLACK, WHITE, 16, 0);
		}
		else
		{
			LCD_ShowString(0, 10 + 22 * 4, Strf("%.2f", CalResult), BLACK, WHITE, 24, 0);
		}
	}
	else
	{
		LCD_ShowString(0, 10 + 22 * 4, "SYNTAX ERROR", RED, WHITE, 24, 0);
	}
}
void C_CalKey(void)
{
	if (KeyNum == 14 && KeyState == 2) // 长按退出
	{
		Cal_Exit();
		C_ShowIndex = 0;
		LCD_Fill(0, 0, 160, 128, WHITE);
		C_MenuShow();
		return;
	}
	uint8_t temp = 1;
	if (C_Cal_State == 1) // 输入状态
	{
		switch (KeyNum)
		{
		case 1:
			temp = Cal_StrInAdd('7');
			break;
		case 2:
			temp = Cal_StrInAdd('8');
			break;
		case 3:
			temp = Cal_StrInAdd('9');
			break;
		case 4:
			temp = Cal_StrInAdd('/');
			break;
		case 5:
			temp = Cal_StrInAdd('4');
			break;
		case 6:
			temp = Cal_StrInAdd('5');
			break;
		case 7:
			temp = Cal_StrInAdd('6');
			break;
		case 8:
			temp = Cal_StrInAdd('*');
			break;
		case 9:
			temp = Cal_StrInAdd('1');
			break;
		case 10:
			temp = Cal_StrInAdd('2');
			break;
		case 11:
			temp = Cal_StrInAdd('3');
			break;
		case 12:
			temp = Cal_StrInAdd('-');
			break;
		case 13:
			temp = Cal_StrInAdd('0');
			break;
		case 14:
			temp = Cal_StrInAdd('.');
			break;
		case 15:
			temp = Cal_StrInAdd('=');
			C_Cal_State = 2;
			break;
		case 16:
			temp = Cal_StrInAdd('+');
			break;
		default:
			break;
		}
		if (temp == 0)
		{
			C_CalShowRe();
		}
		if (C_Cal_State == 2)
		{
			C_Cal_ShowRes();
		}
	}
	else if (C_Cal_State == 2 && KeyNum == 15)
	{
		Cal_Exit();
		LCD_Fill(0, 0, 160, 128, WHITE);
		C_CalShow();
	}
}

/****************************************************************/
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
}
/**
 * @brief 显示记录的时间
 */
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

/*	通用	*/

/**
 * @brief 格式化字符串
 */
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
