#ifndef _KEY_H_
#define _KEY_H_

void Key_Init(void);
void Key_Tick(void);//更新键码;将该函数加入定时器中断函数
uint8_t Key_GetNum(uint8_t *State);//获取键码

#endif
