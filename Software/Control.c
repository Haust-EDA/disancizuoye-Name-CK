#include "stm32f10x.h"
#include <string.h>

#include "LCD.h"
#include "Delay.h"
#include "Control.h"
#include "LED.h"
#include "Calculator.h"
#include "Stopwatch.h"
#include "Weather.h"

uint8_t C_ShowIndex = 0; // 当前显示页面编号

extern uint32_t TimeTick;
extern uint8_t KeyNum;
extern uint8_t KeyState;

/****************************************************************/
/*	主菜单	*/
uint8_t C_MenuShow_Index = 0;
uint8_t C_MenuShow_weatherisget = 0;
/**
 * @brief 显示主菜单
 */
void C_MenuShow(void) // C_ShowIndex->0
{
	LCD_ShowString(5 + 12, 5 + 30 * 0, "LED", BLACK, WHITE, 24, 0);
	LCD_ShowChinese(5 + 12 * 4, 5, "灯", BLACK, WHITE, 24, 0);
	LCD_ShowChinese(5 + 12, 5 + 30 * 1, "计算器", BLACK, WHITE, 24, 0);
	LCD_ShowChinese(5 + 12, 5 + 30 * 2, "秒表", BLACK, WHITE, 24, 0);
	LCD_ShowChinese(5 + 12, 5 + 30 * 3, "天气", WeatherIsGet ? BLACK : GRAY, WHITE, 24, 0);
	LCD_ShowChar(5, 5 + 30 * C_MenuShow_Index, '>', BLACK, WHITE, 24, 0);
}
void C_MenuShowData(void) // 菜单显示_数据刷新
{
	LCD_Fill(5, 5, 5 + 12, 5 + 30 * 3 + 24, WHITE);
	LCD_ShowChar(5, 5 + 30 * C_MenuShow_Index, '>', BLACK, WHITE, 24, 0);
}
void C_MenuKey(void)
{
	switch (KeyNum)
	{
	case 2: // 上
		if (C_MenuShow_Index > 0)
		{
			C_MenuShow_Index--;
		}
		C_MenuShowData();
		break;
	case 10: // 下
		if (C_MenuShow_Index < 3 && !(C_MenuShow_Index == 2 && WeatherIsGet == 0))
		{
			C_MenuShow_Index++;
		}
		C_MenuShowData();

		break;
	case 15: // 进入

		switch (C_MenuShow_Index) // 当前选项
		{
		case 0: // LED灯
			C_ShowIndex = 1;
			LCD_Fill(0, 0, 160, 128, WHITE);
			C_LEDShow();
			break;
		case 1: // 计算器
			C_ShowIndex = 2;
			LCD_Fill(0, 0, 160, 128, WHITE);
			C_CalShow();
			break;
		case 2: // 秒表
			C_ShowIndex = 3;
			LCD_Fill(0, 0, 160, 128, WHITE);
			C_SWShow();
			break;
		case 3: // 天气
			C_ShowIndex = 4;
			LCD_Fill(0, 0, 160, 128, WHITE);
			C_WeatherShow();
			break;
		default:
			break;
		}

	default:
		break;
	}
}

/****************************************************************/
/*	LED		*/
void C_LEDShowData(void);
uint8_t C_LEDShow_Index = 0;

void C_LEDShow(void) // C_ShowIndex->1
{
	LCD_ShowString(0, 0, "LED", GREEN, WHITE, 12, 0); // 标题
	LCD_ShowString(5 + 8, 15 + 22 * 0, "C_Mode", BLACK, WHITE, 16, 0);
	LCD_ShowString(5 + 8, 15 + 22 * 1, "C_Dir", BLACK, WHITE, 16, 0);
	LCD_ShowString(5 + 8, 15 + 22 * 2, "C_Speed", BLACK, WHITE, 16, 0);
	LCD_ShowString(5 + 8, 15 + 22 * 3, "B_Mode", BLACK, WHITE, 16, 0);
	C_LEDShowData();
	extern uint32_t LED_TimeStart;
	LED_TimeStart = TimeTick;
}
void C_LEDShowData(void)
{

	for (uint8_t i = 0; i < 4; i++)
	{
		LCD_ShowChar(5, 15 + 22 * i, (i == C_LEDShow_Index) ? '>' : ' ', BLACK, WHITE, 16, 0);
		LCD_ShowString(5 + 8 * 10, 15 + 22 * i, "<     >", (i == C_LEDShow_Index) ? RED : BLACK, WHITE, 16, 0);
	}
	// CMode
	switch (LEDCMode)
	{
	case 0:
		LCD_ShowString(5 + 8 * 12, 15 + 22 * 0, "OFF", BLACK, WHITE, 16, 0);
		break;
	case 1:
		LCD_ShowChar(5 + 8 * 13, 15 + 22 * 0, '1', BLACK, WHITE, 16, 0);
		break;
	case 2:
		LCD_ShowChar(5 + 8 * 13, 15 + 22 * 0, '2', BLACK, WHITE, 16, 0);
		break;
	case 3:
		LCD_ShowChar(5 + 8 * 13, 15 + 22 * 0, '3', BLACK, WHITE, 16, 0);
		break;
	case 4:
		LCD_ShowChar(5 + 8 * 13, 15 + 22 * 0, '4', BLACK, WHITE, 16, 0);
		break;
	case 5:
		LCD_ShowString(5 + 8 * 12, 15 + 22 * 0, "ALL", BLACK, WHITE, 16, 0);
		break;
	default:
		break;
	}
	// CDir
	if (LEDCDir == 0)
	{
		LCD_ShowString(5 + 8 * 12.5, 15 + 22 * 1, "CW", BLACK, WHITE, 16, 0);
	}
	else
	{
		LCD_ShowString(5 + 8 * 12, 15 + 22 * 1, "CCW", BLACK, WHITE, 16, 0);
	}
	// CSpeed
	LCD_ShowString(5 + 8 * 12.5, 15 + 22 * 2, Strf("%0.2d", LEDCSpeed), BLACK, WHITE, 16, 0);
	// BMode
	switch (LEDBMode)
	{
	case 0:
		LCD_ShowString(5 + 8 * 12, 15 + 22 * 3, "OFF", BLACK, WHITE, 16, 0);
		break;
	case 1:
		LCD_ShowChar(5 + 8 * 13, 15 + 22 * 3, '1', BLACK, WHITE, 16, 0);
		break;
	case 2:
		LCD_ShowChar(5 + 8 * 13, 15 + 22 * 3, '2', BLACK, WHITE, 16, 0);
		break;
	default:
		break;
	}
}
void C_LEDKey(void)
{
	if (KeyNum == 14 && KeyState == 2) // 长按退出
	{
		C_ShowIndex = 0;
		LCD_Fill(0, 0, 160, 128, WHITE);
		C_MenuShow();
		return;
	}

	switch (KeyNum)
	{
	case 5: // 左
		switch (C_LEDShow_Index)
		{
		case 0: // C_Mode
			LED_SetCMode((LEDCMode - 1 + 6) % 6);
			// LEDCMode = (LEDCMode - 1 + 6) % 6;
			break;
		case 1: // C_Dir
			LED_SetCDir((LEDCDir - 1 + 2) % 2);
			// LEDCDir = (LEDCDir - 1 + 2) % 2;
			break;
		case 2: // C_Speed
			LED_SetCSpeed((LEDCSpeed - 2 + 99) % 99 + 1);
			// LEDCSpeed = (LEDCSpeed - 2 + 99) % 99 + 1;
			break;
		case 3: // B_Mode
			LED_SetBMode((LEDBMode - 1 + 3) % 3);
			// LEDBMode = (LEDBMode - 1 + 3) % 3;
			break;
		default:
			break;
		}
		break;

	case 7: // 右
		switch (C_LEDShow_Index)
		{
		case 0: // C_Mode
			LED_SetCMode((LEDCMode + 1) % 6);
			// LEDCMode = (LEDCMode + 1) % 6;
			break;
		case 1: // C_Dir
			LED_SetCDir((LEDCDir + 1) % 2);
			// LEDCDir = (LEDCDir + 1) % 2;
			break;
		case 2: // C_Speed
			LED_SetCSpeed((LEDCSpeed + 2) % 99 - 1);
			// LEDCSpeed = LEDCSpeed % 99 - 1;
			break;
		case 3: // B_Mode
			LED_SetBMode((LEDBMode + 1) % 3);
			// LEDBMode = (LEDBMode + 1) % 3;
			break;
		default:
			break;
		}
		break;
	case 2: // 上
		C_LEDShow_Index = (C_LEDShow_Index - 1 + 4) % 4;
		break;
	case 10: // 下
		C_LEDShow_Index = (C_LEDShow_Index + 1) % 4;
		break;
	default:
		break;
	}
	C_LEDShowData();
}

/****************************************************************/
/*	计算器	*/
#define _C_CAL_SHOW_ROW 4	  // 行数
#define _C_CAL_SHOW_COL 10	  // 列数
uint8_t C_Cal_State = 0;	  // 计算器状态(1:输入状态,2:结果状态,3:历史状态)
uint8_t C_CalShow_HIndex = 0; // 历史显示索引

void C_CalShowRefesh(void); // 显示刷新

void C_CalShow(void) // C_ShowIndex->2
{
	C_Cal_State = 1;
	LCD_ShowString(0, 0, "Calculator", GREEN, WHITE, 12, 0); // 标题
	C_CalShowRefesh();
}
/**
 * @brief 显示内容刷新
 */
void C_CalShowRefesh(void)
{
	if (C_Cal_State == 1)
	{
		uint8_t showIndex = 0;
		uint8_t showRow = 0;
		uint8_t showCol = 0;

		while (CalStrIn[showIndex] != '\0')
		{
			LCD_ShowChar(0 + 16 * showCol, 10 + 22 * showRow, CalStrIn[showIndex], BLACK, WHITE, 24, 0);
			showIndex++;
			showCol++;
			if (showCol == _C_CAL_SHOW_COL)
			{
				showCol = 0;
				showRow++;
			}
		}
		if (CalStrIn_Index != STRIN_MAX_LENGTH - 1)
		{
			LCD_ShowChar(0 + 16 * showCol, 10 + 22 * showRow, '<', RED, WHITE, 24, 0);
			showIndex++;
			showCol++;
			if (showCol == _C_CAL_SHOW_COL)
			{
				showCol = 0;
				showRow++;
			}
			LCD_ShowChar(0 + 16 * showCol, 10 + 22 * showRow, ' ', WHITE, WHITE, 24, 0);
		}
	}
}
/**
 * @brief 显示结果
 */
void C_Cal_ShowRes(void)
{
	if (Cal_Run() != 0)
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

void C_Cal_ShowHistory(void)
{
	LCD_ShowString(0, 0, "Calculator_History", GREEN, WHITE, 12, 0); // 标题
	uint8_t show_row = 0, show_col = 0;
	char show_str[7][21] = {"\0", "\0", "\0", "\0", "\0", "\0", "\0"};
	uint8_t show_strIndex = 0;

	while (show_strIndex != CalHStrIndex[C_CalShow_HIndex - 1])
	{
		show_str[show_row][show_col] = CalHStr[C_CalShow_HIndex - 1][show_strIndex];
		show_col++;
		if (show_col == 20)
		{
			show_str[show_row][show_col] = '\0';
			show_col = 0;
			show_row++;
		}
		show_strIndex++;
	}
	show_str[show_row][show_col] = '\0';

	for (uint8_t i = 0; (i < C_CalShow_HIndex) && (i < 4); i++)
	{
		strcpy(show_str[3 + i], (char *)Strf("%.2f", CalHRes[C_CalShow_HIndex - 1 - i]));
	}

	LCD_DrawLine(0, 13, 160, 13, BLACK);
	for (uint8_t i = 0; i < 3; i++)
	{
		LCD_ShowString(0, 14 + 16 * i, (uint8_t *)show_str[i], BLACK, WHITE, 16, 0);
	}
	LCD_ShowString(0, 14 + 16 * 3, (uint8_t *)show_str[3], RED, WHITE, 16, 0);
	LCD_DrawLine(0, 14 + 16 * 4 + 1, 160, 14 + 16 * 4 + 1, BLACK);

	for (uint8_t i = 4; i < 7; i++)
	{
		LCD_ShowString(0, 14 + 3 + 16 * i, (uint8_t *)show_str[i], BLACK, WHITE, 16, 0);
	}
}

void C_CalKey(void)
{
	uint8_t temp = 1;
	if (C_Cal_State == 1) // 输入状态
	{
		if (KeyState == 1) // 短按
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
			case 15: // 计算
				C_Cal_State = 2;
				break;
			case 16:
				temp = Cal_StrInAdd('+');
				break;
			default:
				break;
			}
		}
		else if (KeyState == 2) // 长按
		{
			switch (KeyNum)
			{
			case 1: // 查看历史记录
				LCD_Fill(0, 0, 160, 128, WHITE);
				C_CalShow_HIndex = CalHIndex;
				C_Cal_ShowHistory();
				C_Cal_State = 3;
				return;
				// break;
			case 2: // 阶乘fac
				Cal_StrInAdd('f');
				Cal_StrInAdd('a');
				Cal_StrInAdd('c');
				temp = Cal_StrInAdd('(');
				break;
			case 3: // sin
				Cal_StrInAdd('s');
				Cal_StrInAdd('i');
				Cal_StrInAdd('n');
				temp = Cal_StrInAdd('(');
				break;
			case 4: // (
				temp = Cal_StrInAdd('(');
				break;
			case 5:
				temp = Cal_StrInDel();
				break;
			case 6: // 排列
				Cal_StrInAdd('n');
				Cal_StrInAdd('c');
				Cal_StrInAdd('r');
				temp = Cal_StrInAdd('(');
				break;
			case 7: // cos
				Cal_StrInAdd('c');
				Cal_StrInAdd('o');
				Cal_StrInAdd('s');
				temp = Cal_StrInAdd('(');
				break;
			case 8: // )
				temp = Cal_StrInAdd(')');
				break;
			case 10: // 组合
				Cal_StrInAdd('n');
				Cal_StrInAdd('p');
				Cal_StrInAdd('r');
				temp = Cal_StrInAdd('(');
				break;
			case 11: // tan
				Cal_StrInAdd('t');
				Cal_StrInAdd('a');
				Cal_StrInAdd('n');
				temp = Cal_StrInAdd('(');
				break;
			case 12: // ^
				temp = Cal_StrInAdd('^');
				break;
			case 13: // ,
				temp = Cal_StrInAdd(',');
				break;
			case 14: // 退出
				Cal_Exit();
				C_ShowIndex = 0;
				LCD_Fill(0, 0, 160, 128, WHITE);
				C_MenuShow();
				return;
				// break;
			case 15: // pi
				Cal_StrInAdd('p');
				temp = Cal_StrInAdd('i');
				break;
			case 16: // 取模
				temp = Cal_StrInAdd('%');
				break;
			default:
				break;
			}
		}
		if (temp == 0)
		{
			C_CalShowRefesh();
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
	else if (C_Cal_State == 3) // 处于 查看历史 状态
	{
		switch (KeyNum)
		{
		case 1:
			LCD_Fill(0, 0, 160, 128, WHITE);
			CalStrIn_Index = 0;
			CalStrIn[0] = '\0';
			C_CalShow();
			C_Cal_State = 1;

			break;
		case 2:
			if (C_CalShow_HIndex < CalHIndex)
			{
				C_CalShow_HIndex++;
				LCD_Fill(0, 0, 160, 128, WHITE);
				C_Cal_ShowHistory();
			}

			break;
		case 10:
			if (C_CalShow_HIndex > 1)
			{
				C_CalShow_HIndex--;
				LCD_Fill(0, 0, 160, 128, WHITE);
				C_Cal_ShowHistory();
			}
		default:
			break;
		}
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

/****************************************************************/
/*	天气	*/
uint8_t C_Weather_csatIndex = 0;
void C_WeatherShow(void) // C_ShowIndex->4
{
	/*固定内容*/
	LCD_ShowChinese(0, 0, "洛阳", BLACK, WHITE, 24, 0);
	LCD_DrawLine(49, 0, 49, 25, BLACK);
	LCD_DrawLine(0, 25, 160, 25, BLACK);
	// 白天
	LCD_ShowChinese24x24(0, 26, "白", BLACK, WHITE, 24, 0);
	LCD_ShowChinese24x24(0, 52, "天", BLACK, WHITE, 24, 0);
	LCD_DrawLine(0, 76, 160, 76, BLACK);
	// 夜晚
	LCD_ShowChinese24x24(0, 78, "夜", BLACK, WHITE, 24, 0);
	LCD_ShowChinese24x24(0, 104, "晚", BLACK, WHITE, 24, 0);
	// 其它
	LCD_DrawLine(25, 25, 25, 128, BLACK); // 左边竖线
	// 数据
	C_WeatherShowData();
}
void C_WeatherShowData(void)
{
	/*清空屏幕原有内容 */
	LCD_Fill(26, 26, 160, 75, WHITE);
	LCD_Fill(26, 78, 160, 128, WHITE);
	/*显示数据*/
	cast_t *cast = &WeatherCasts[C_Weather_csatIndex];
	// 日期
	LCD_ShowString(52, 0, (uint8_t *)((cast->date) + 5), C_Weather_csatIndex == 0 ? RED : BLACK, WHITE, 24, 0);
	const char *week[] = {"一", "二", "三", "四", "五", "六", "日"};
	LCD_ShowChinese(112, 0, "周", C_Weather_csatIndex == 0 ? RED : BLACK, WHITE, 24, 0);
	LCD_ShowChinese(136, 0, (uint8_t *)week[(cast->week)[0] - 48 - 1], C_Weather_csatIndex == 0 ? RED : BLACK, WHITE, 24, 0);

	// 白天
	// 天气
	if (strlen((cast->dayweather)) / 3 > 3)
	{
		LCD_ShowChinese(26, 26, "未知", BLACK, WHITE, 24, 0);
	}
	else
	{
		LCD_ShowChinese(26, 26, (uint8_t *)(cast->dayweather), BLACK, WHITE, 24, 0);
	}
	// 温度
	LCD_ShowString(100, 26, Strf("%3s", (cast->daytemp)), BLACK, WHITE, 24, 0);
	LCD_ShowChinese(136, 26, "℃", BLACK, WHITE, 24, 0);
	// 风向
	if (strlen((cast->daywind)) / 3 <= 2)
	{
		LCD_ShowChinese(26, 26 + 26, (uint8_t *)(cast->daywind), BLACK, WHITE, 24, 0);
		LCD_ShowChinese(26 + (strlen((cast->nightwind)) / 3) * 24, 26 + 26, "风", BLACK, WHITE, 24, 0);
	}
	else if (strlen((cast->daywind)) / 3 >= 4)
	{
		LCD_ShowChinese(26, 26 + 26, "不定", BLACK, WHITE, 24, 0);
	}
	// 风力
	LCD_ShowString(100, 26 + 26, Strf("%3s", (cast->daypower)), BLACK, WHITE, 24, 0);
	LCD_ShowChinese(136, 52, "级", BLACK, WHITE, 24, 0);

	// 夜晚
	// 天气
	if (strlen((cast->nightweather)) / 3 > 3)
	{
		LCD_ShowChinese(26, 78, "未知", BLACK, WHITE, 24, 0);
	}
	else
	{
		LCD_ShowChinese(26, 78, (uint8_t *)(cast->nightweather), BLACK, WHITE, 24, 0);
	}
	// 温度
	LCD_ShowString(100, 78, Strf("%3s", (cast->nighttemp)), BLACK, WHITE, 24, 0);
	LCD_ShowChinese(136, 78, "℃", BLACK, WHITE, 24, 0);
	// 风向
	if (strlen((cast->nightwind)) / 3 <= 2)
	{
		LCD_ShowChinese(26, 78 + 26, (uint8_t *)(cast->nightwind), BLACK, WHITE, 24, 0);
		LCD_ShowChinese(26 + (strlen((cast->nightwind)) / 3) * 24, 78 + 26, "风", BLACK, WHITE, 24, 0);
	}
	else if (strlen((cast->nightwind)) / 3 >= 4)
	{
		LCD_ShowChinese(26, 78 + 26, "不定", BLACK, WHITE, 24, 0);
	}
	// 风力
	LCD_ShowString(100, 78 + 26, Strf("%3s", (cast->nightpower)), BLACK, WHITE, 24, 0);
	LCD_ShowChinese(136, 104, "级", BLACK, WHITE, 24, 0);
}

void C_WeatherKey(void)
{
	switch (KeyNum)
	{
	case 5:
		if (C_Weather_csatIndex != 0)
		{
			C_Weather_csatIndex--;
			C_WeatherShowData();
		}
		break;
	case 7:
		if (C_Weather_csatIndex != 3)
		{
			C_Weather_csatIndex++;
			C_WeatherShowData();
		}
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

/****************************************************************/
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
void C_Refresh(void)
{
	if (C_ShowIndex == 0 && C_MenuShow_weatherisget == 0 && WeatherIsGet == 1)
	{
		C_MenuShow_weatherisget = 1;
		C_MenuShow();
	}
	if (C_ShowIndex == 3 && SW_Flag == 1)
	{
		C_SWShowTime0();
	}
	LED_Refresh();
}
