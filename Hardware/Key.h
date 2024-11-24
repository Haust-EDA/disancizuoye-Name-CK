#ifndef __KEY_H_
#define __KEY_H_

void Key_Init(void);
uint8_t Key_GetNum(void);//获取键码
void Key_Tick(void);//更新键码;将该函数加入定时器中断函数

#endif
