#include "stm32f10x.h"                  // Device header
#include "OLED_UI.h"
#include "Gvar.h"
#include "Key.h"
void Control_Key(void)
{
	switch (G_KeyNum)
	{
	case 1:G_UIState=1;
		break;
	
	default:
		break;
	}
}

void Control_Tick(void)
{
	Key_Tick();	
	
}
void Control_Main(void)
{
	G_KeyNum=Key_GetNum();
	if (G_KeyNum!=0)
	{
		Control_Key();
	}
}
