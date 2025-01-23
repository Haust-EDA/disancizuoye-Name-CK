#ifndef __CONTROL_H_
#define __CONTROL_H_
#include "stdint.h"
#include <stdio.h>
#include <stdarg.h>

extern uint8_t C_ShowIndex;
/*------------  主菜单  ------------*/

void C_MenuShow(void);//菜单显示
void C_MenuKey(void);//菜单按键处理
/*------------  计算器  ------------*/

void C_CalShow(void);
void C_CalKey(void);


/*------------  秒表    ------------*/

void C_SWShow(void);//秒表显示
void C_SWKey(void);//秒表按键处理

/*------------  通用    ------------*/

void C_ShowRe(void);
uint8_t* Strf(char* format, ...);

#endif
