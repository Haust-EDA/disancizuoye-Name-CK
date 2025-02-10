#include "stm32f10x.h"
#include "LCD.h"
#include "stdlib.h"
#include "LCDfont.h"
#include "Delay.h"

/**
 * @brief  初始化 SPI 外设(SCK,MOSI)
 */
void LCD_SPI_Init(void)
{
	// 启用 SPI 外设和 GPIO 外设时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

	// 配置 SPI 引脚：(MCU)PA5->(TFT)SCK, (MCU)PA7->(TFT)MOSI
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7; // SPI1 SCK 和 MOSI
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		   // 复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// 配置 SPI 外设
	SPI_InitTypeDef SPI_InitStructure;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2; // SPI 时钟速率
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;				   // 数据位宽 8位
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;						   // 时钟空闲时为低电平
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;					   // 数据在时钟的第一个边沿采样
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;						   // 软件 NSS 控制
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;					   // 设置 SPI 为主模式
	SPI_Init(SPI1, &SPI_InitStructure);

	// 启用 SPI 外设
	SPI_Cmd(SPI1, ENABLE);
}
/**
 * @brief 初始化 GPIO(BLK,CS,DC,RST)
 */
void LCD_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // 使能A端口时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); // 使能C端口时钟

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // 推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // 速度50MHz

	GPIO_InitStructure.GPIO_Pin = LCD_BLK_GPIO_PIN; // BLK
	GPIO_Init(LCD_BLK_GPIO_PORT, &GPIO_InitStructure);
	GPIO_SetBits(LCD_BLK_GPIO_PORT, LCD_BLK_GPIO_PIN);
	GPIO_InitStructure.GPIO_Pin = LCD_CS_GPIO_PIN; // CS
	GPIO_Init(LCD_CS_GPIO_PORT, &GPIO_InitStructure);
	GPIO_SetBits(LCD_CS_GPIO_PORT, LCD_CS_GPIO_PIN);
	GPIO_InitStructure.GPIO_Pin = LCD_DC_GPIO_PIN; // DC
	GPIO_Init(LCD_DC_GPIO_PORT, &GPIO_InitStructure);
	GPIO_SetBits(LCD_DC_GPIO_PORT, LCD_DC_GPIO_PIN);
	GPIO_InitStructure.GPIO_Pin = LCD_RST_GPIO_PIN; // RST
	GPIO_Init(LCD_RST_GPIO_PORT, &GPIO_InitStructure);
	GPIO_SetBits(LCD_RST_GPIO_PORT, LCD_RST_GPIO_PIN);
}

/**
 * @brief LCD串行数据写入函数
 * @param dat 要写入的串行数据
 * @return 无
 */

void LCD_Writ_Bus(uint8_t dat)
{
	// 1. 使能片选，开始通信
	LCD_CS_Clr();

	// 2. 发送数据到 SPI
	SPI1->DR = dat;
	// SPI_I2S_SendData(SPI1, dat); // 将数据写入 SPI 数据寄存器，启动发送

	// 3. 等待数据传输完成
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
		; // 等待发送缓冲区为空
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET)
		; // 等待 SPI 总线空闲

	// 4. 取消片选，结束通信
	LCD_CS_Set();
}

/**
 * @brief  LCD写入数据
 * @param  dat 要写入的数据
 * @return 无
 */
void LCD_WR_DATA8(uint8_t dat)
{
	LCD_Writ_Bus(dat);
}

/**
 * @brief  LCD写入数据
 * @param  dat 要写入的数据
 * @return 无
 */
void LCD_WR_DATA(uint16_t dat)
{
	LCD_Writ_Bus(dat >> 8);
	LCD_Writ_Bus(dat);
}

/**
 * @brief  LCD写入命令
 * @param  dat 要写入的命令
 * @return 无
 */
void LCD_WR_REG(uint8_t dat)
{
	LCD_DC_Clr(); // 写命令
	LCD_Writ_Bus(dat);
	LCD_DC_Set(); // 写数据
}

/**
 * @brief 	设置起始和结束地址
 * @param	x1 列的起始地址
 * @param	y1 行的起始地址
 * @param	x2 列的结束地址
 * @param	y2 行的结束地址
 * @return	无
 */
void LCD_Address_Set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	if (USE_HORIZONTAL == 0)
	{
		LCD_WR_REG(0x2a); // 列地址设置
		LCD_WR_DATA(x1);
		LCD_WR_DATA(x2);
		LCD_WR_REG(0x2b); // 行地址设置
		LCD_WR_DATA(y1);
		LCD_WR_DATA(y2);
		LCD_WR_REG(0x2c); // 储存器写
	}
	else if (USE_HORIZONTAL == 1)
	{
		LCD_WR_REG(0x2a); // 列地址设置
		LCD_WR_DATA(x1);
		LCD_WR_DATA(x2);
		LCD_WR_REG(0x2b); // 行地址设置
		LCD_WR_DATA(y1);
		LCD_WR_DATA(y2);
		LCD_WR_REG(0x2c); // 储存器写
	}
	else if (USE_HORIZONTAL == 2)
	{
		LCD_WR_REG(0x2a); // 列地址设置
		LCD_WR_DATA(x1);
		LCD_WR_DATA(x2);
		LCD_WR_REG(0x2b); // 行地址设置
		LCD_WR_DATA(y1);
		LCD_WR_DATA(y2);
		LCD_WR_REG(0x2c); // 储存器写
	}
	else
	{
		LCD_WR_REG(0x2a); // 列地址设置
		LCD_WR_DATA(x1);
		LCD_WR_DATA(x2);
		LCD_WR_REG(0x2b); // 行地址设置
		LCD_WR_DATA(y1);
		LCD_WR_DATA(y2);
		LCD_WR_REG(0x2c); // 储存器写
	}
}

/**
 * @brief LCD初始化
 */
void LCD_Init(void)
{
	// 先初始化SPI再初始化GPIO
	LCD_SPI_Init();	 // 初始化SPI(及其GPIO)
	LCD_GPIO_Init(); // 初始化GPIO

	LCD_RES_Clr(); // 复位
	Delay_ms(100);
	LCD_RES_Set();
	Delay_ms(100);

	LCD_BLK_Set(); // 打开背光
	Delay_ms(100);

	//************* Start Initial Sequence **********//
	LCD_WR_REG(0x11); // Sleep out
	Delay_ms(120);	  // Delay 120ms
	//------------------------------------ST7735S Frame Rate-----------------------------------------//
	LCD_WR_REG(0xB1);
	LCD_WR_DATA8(0x05);
	LCD_WR_DATA8(0x3C);
	LCD_WR_DATA8(0x3C);
	LCD_WR_REG(0xB2);
	LCD_WR_DATA8(0x05);
	LCD_WR_DATA8(0x3C);
	LCD_WR_DATA8(0x3C);
	LCD_WR_REG(0xB3);
	LCD_WR_DATA8(0x05);
	LCD_WR_DATA8(0x3C);
	LCD_WR_DATA8(0x3C);
	LCD_WR_DATA8(0x05);
	LCD_WR_DATA8(0x3C);
	LCD_WR_DATA8(0x3C);
	//------------------------------------End ST7735S Frame Rate---------------------------------//
	LCD_WR_REG(0xB4); // Dot inversion
	LCD_WR_DATA8(0x03);
	//------------------------------------ST7735S Power Sequence---------------------------------//
	LCD_WR_REG(0xC0);
	LCD_WR_DATA8(0x28);
	LCD_WR_DATA8(0x08);
	LCD_WR_DATA8(0x04);
	LCD_WR_REG(0xC1);
	LCD_WR_DATA8(0XC0);
	LCD_WR_REG(0xC2);
	LCD_WR_DATA8(0x0D);
	LCD_WR_DATA8(0x00);
	LCD_WR_REG(0xC3);
	LCD_WR_DATA8(0x8D);
	LCD_WR_DATA8(0x2A);
	LCD_WR_REG(0xC4);
	LCD_WR_DATA8(0x8D);
	LCD_WR_DATA8(0xEE);
	//---------------------------------End ST7735S Power Sequence-------------------------------------//
	LCD_WR_REG(0xC5); // VCOM
	LCD_WR_DATA8(0x1A);
	LCD_WR_REG(0x36); // MX, MY, RGB mode
	if (USE_HORIZONTAL == 0)
		LCD_WR_DATA8(0x00);
	else if (USE_HORIZONTAL == 1)
		LCD_WR_DATA8(0xC0);
	else if (USE_HORIZONTAL == 2)
		LCD_WR_DATA8(0x70);
	else
		LCD_WR_DATA8(0xA0);
	//------------------------------------ST7735S Gamma Sequence---------------------------------//
	LCD_WR_REG(0xE0);
	LCD_WR_DATA8(0x04);
	LCD_WR_DATA8(0x22);
	LCD_WR_DATA8(0x07);
	LCD_WR_DATA8(0x0A);
	LCD_WR_DATA8(0x2E);
	LCD_WR_DATA8(0x30);
	LCD_WR_DATA8(0x25);
	LCD_WR_DATA8(0x2A);
	LCD_WR_DATA8(0x28);
	LCD_WR_DATA8(0x26);
	LCD_WR_DATA8(0x2E);
	LCD_WR_DATA8(0x3A);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x01);
	LCD_WR_DATA8(0x03);
	LCD_WR_DATA8(0x13);
	LCD_WR_REG(0xE1);
	LCD_WR_DATA8(0x04);
	LCD_WR_DATA8(0x16);
	LCD_WR_DATA8(0x06);
	LCD_WR_DATA8(0x0D);
	LCD_WR_DATA8(0x2D);
	LCD_WR_DATA8(0x26);
	LCD_WR_DATA8(0x23);
	LCD_WR_DATA8(0x27);
	LCD_WR_DATA8(0x27);
	LCD_WR_DATA8(0x25);
	LCD_WR_DATA8(0x2D);
	LCD_WR_DATA8(0x3B);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x01);
	LCD_WR_DATA8(0x04);
	LCD_WR_DATA8(0x13);
	//------------------------------------End ST7735S Gamma Sequence-----------------------------//
	LCD_WR_REG(0x3A); // 65k mode
	LCD_WR_DATA8(0x05);
	LCD_WR_REG(0x29); // Display on
}

/**
 * @brief 	填充颜色
 * @param	xsta 列的起始地址
 * @param	ysta 行的起始地址
 * @param	xend 列的结束地址
 * @param	yend 行的结束地址
 * @param	color 要填充的颜色
 * @return	无
 */
void LCD_Fill(uint16_t xsta, uint16_t ysta, uint16_t xend, uint16_t yend, uint16_t color)
{
	uint16_t i, j;
	LCD_Address_Set(xsta, ysta, xend - 1, yend - 1); // 设置显示范围
	for (i = ysta; i < yend; i++)
	{
		for (j = xsta; j < xend; j++)
		{
			LCD_WR_DATA(color);
		}
	}
}

/**
 * @brief 	在指定位置画点
 * @param	x 列的坐标
 * @param	y 行的坐标
 * @param	color 点的颜色
 * @return	无
 */
void LCD_DrawPoint(uint16_t x, uint16_t y, uint16_t color)
{
	LCD_Address_Set(x, y, x, y); // 设置光标位置
	LCD_WR_DATA(color);
}

/**
 * @brief 	画线
 * @param	x1 起始点的列坐标
 * @param	y1 起始点的行坐标
 * @param	x2 终止点的列坐标
 * @param	y2 终止点的行坐标
 * @param	color 线的颜色
 * @return	无
 */
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
	uint16_t t;
	int xerr = 0, yerr = 0, delta_x, delta_y, distance;
	int incx, incy, uRow, uCol;
	delta_x = x2 - x1; // 计算坐标增量
	delta_y = y2 - y1;
	uRow = x1; // 画线起点坐标
	uCol = y1;
	if (delta_x > 0)
		incx = 1; // 设置单步方向
	else if (delta_x == 0)
		incx = 0; // 垂直线
	else
	{
		incx = -1;
		delta_x = -delta_x;
	}
	if (delta_y > 0)
		incy = 1;
	else if (delta_y == 0)
		incy = 0; // 水平线
	else
	{
		incy = -1;
		delta_y = -delta_y;
	}
	if (delta_x > delta_y)
		distance = delta_x; // 选取基本增量坐标轴
	else
		distance = delta_y;
	for (t = 0; t < distance + 1; t++)
	{
		LCD_DrawPoint(uRow, uCol, color); // 画点
		xerr += delta_x;
		yerr += delta_y;
		if (xerr > distance)
		{
			xerr -= distance;
			uRow += incx;
		}
		if (yerr > distance)
		{
			yerr -= distance;
			uCol += incy;
		}
	}
}

/**
 * @brief	画矩形
 * @param	x1 矩形左上角的列坐标
 * @param	y1 矩形左上角的行坐标
 * @param	x2 矩形右下角的列坐标
 * @param	y2 矩形右下角的行坐标
 * @param	color 矩形的颜色
 * @return	无
 */
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
	LCD_DrawLine(x1, y1, x2, y1, color);
	LCD_DrawLine(x1, y1, x1, y2, color);
	LCD_DrawLine(x1, y2, x2, y2, color);
	LCD_DrawLine(x2, y1, x2, y2, color);
}

/**
 * @brief	画圆
 * @param	x0 圆心的列坐标
 * @param	y0 圆心的行坐标
 * @param	r  半径
 * @param	color 圆的颜色
 * @return	无
 */
void Draw_Circle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color)
{
	int a, b;
	a = 0;
	b = r;
	while (a <= b)
	{
		LCD_DrawPoint(x0 - b, y0 - a, color); // 3
		LCD_DrawPoint(x0 + b, y0 - a, color); // 0
		LCD_DrawPoint(x0 - a, y0 + b, color); // 1
		LCD_DrawPoint(x0 - a, y0 - b, color); // 2
		LCD_DrawPoint(x0 + b, y0 + a, color); // 4
		LCD_DrawPoint(x0 + a, y0 - b, color); // 5
		LCD_DrawPoint(x0 + a, y0 + b, color); // 6
		LCD_DrawPoint(x0 - b, y0 + a, color); // 7
		a++;
		if ((a * a + b * b) > (r * r)) // 判断要画的点是否过远
		{
			b--;
		}
	}
}

/**
 * @brief 	显示汉字串
 * @param	x 列的坐标
 * @param	y 行的坐标
 * @param	s 要显示的汉字串
 * @param	fc 字的颜色
 * @param	bc 字的背景色
 * @param	sizey 字号 可选 12 16 24 32
 * @param	mode 0非叠加模式  1叠加模式
 * @return	无
 */
void LCD_ShowChinese(uint16_t x, uint16_t y, uint8_t *s, uint16_t fc, uint16_t bc, uint8_t sizey, uint8_t mode)
{
	while (*s != 0)
	{
		if (sizey == 12)
			LCD_ShowChinese12x12(x, y, s, fc, bc, sizey, mode);
		else if (sizey == 16)
			LCD_ShowChinese16x16(x, y, s, fc, bc, sizey, mode);
		else if (sizey == 24)
			LCD_ShowChinese24x24(x, y, s, fc, bc, sizey, mode);
		else if (sizey == 32)
			LCD_ShowChinese32x32(x, y, s, fc, bc, sizey, mode);
		else
			return;
		s += 3;
		x += sizey;
	}
}

/**
 * @brief 	显示单个12x12汉字
 * @param	x 列的坐标
 * @param	y 行的坐标
 * @param	s 要显示的汉字
 * @param	fc 字的颜色
 * @param	bc 字的背景色
 * @param	sizey 字号
 * @param	mode 0非叠加模式  1叠加模式
 * @return	无
 */
void LCD_ShowChinese12x12(uint16_t x, uint16_t y, uint8_t *s, uint16_t fc, uint16_t bc, uint8_t sizey, uint8_t mode)
{
	uint8_t i, j, m = 0;
	uint16_t k;
	uint16_t HZnum;		  // 汉字数目
	uint16_t TypefaceNum; // 一个字符所占字节大小
	uint16_t x0 = x;
	TypefaceNum = (sizey / 8 + ((sizey % 8) ? 1 : 0)) * sizey;

	HZnum = sizeof(tfont12) / sizeof(typFNT_UTF8_12); // 统计汉字数目
	for (k = 0; k < HZnum; k++)
	{
		if ((tfont12[k].Index[0] == *(s)) && (tfont12[k].Index[1] == *(s + 1)) && (tfont12[k].Index[2] == *(s + 2)))
		{
			LCD_Address_Set(x, y, x + sizey - 1, y + sizey - 1);
			for (i = 0; i < TypefaceNum; i++)
			{
				for (j = 0; j < 8; j++)
				{
					if (!mode) // 非叠加方式
					{
						if (tfont12[k].Msk[i] & (0x01 << j))
							LCD_WR_DATA(fc);
						else
							LCD_WR_DATA(bc);
						m++;
						if (m % sizey == 0)
						{
							m = 0;
							break;
						}
					}
					else // 叠加方式
					{
						if (tfont12[k].Msk[i] & (0x01 << j))
							LCD_DrawPoint(x, y, fc); // 画一个点
						x++;
						if ((x - x0) == sizey)
						{
							x = x0;
							y++;
							break;
						}
					}
				}
			}
		}
		continue; // 查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
	}
}

/**
 * @brief 	显示单个16x16汉字
 * @param	x 列的坐标
 * @param	y 行的坐标
 * @param	s 要显示的汉字
 * @param	fc 字的颜色
 * @param	bc 字的背景色
 * @param	sizey 字号
 * @param	mode 0非叠加模式  1叠加模式
 * @return	无
 */
void LCD_ShowChinese16x16(uint16_t x, uint16_t y, uint8_t *s, uint16_t fc, uint16_t bc, uint8_t sizey, uint8_t mode)
{
	uint8_t i, j, m = 0;
	uint16_t k;
	uint16_t HZnum;		  // 汉字数目
	uint16_t TypefaceNum; // 一个字符所占字节大小
	uint16_t x0 = x;
	TypefaceNum = (sizey / 8 + ((sizey % 8) ? 1 : 0)) * sizey;
	HZnum = sizeof(tfont16) / sizeof(typFNT_UTF8_16); // 统计汉字数目
	for (k = 0; k < HZnum; k++)
	{
		if ((tfont16[k].Index[0] == *(s)) && (tfont16[k].Index[1] == *(s + 1)) && (tfont16[k].Index[2] == *(s + 2)))
		{
			LCD_Address_Set(x, y, x + sizey - 1, y + sizey - 1);
			for (i = 0; i < TypefaceNum; i++)
			{
				for (j = 0; j < 8; j++)
				{
					if (!mode) // 非叠加方式
					{
						if (tfont16[k].Msk[i] & (0x01 << j))
							LCD_WR_DATA(fc);
						else
							LCD_WR_DATA(bc);
						m++;
						if (m % sizey == 0)
						{
							m = 0;
							break;
						}
					}
					else // 叠加方式
					{
						if (tfont16[k].Msk[i] & (0x01 << j))
							LCD_DrawPoint(x, y, fc); // 画一个点
						x++;
						if ((x - x0) == sizey)
						{
							x = x0;
							y++;
							break;
						}
					}
				}
			}
		}
		continue; // 查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
	}
}

/**
 * @brief 	显示单个24x24汉字
 * @param	x 列的坐标
 * @param	y 行的坐标
 * @param	s 要显示的汉字
 * @param	fc 字的颜色
 * @param	bc 字的背景色
 * @param	sizey 字号
 * @param	mode 0非叠加模式  1叠加模式
 * @return	无
 */
void LCD_ShowChinese24x24(uint16_t x, uint16_t y, uint8_t *s, uint16_t fc, uint16_t bc, uint8_t sizey, uint8_t mode)
{
	uint8_t i, j, m = 0;
	uint16_t k;
	uint16_t HZnum;		  // 汉字数目
	uint16_t TypefaceNum; // 一个字符所占字节大小
	uint16_t x0 = x;
	TypefaceNum = (sizey / 8 + ((sizey % 8) ? 1 : 0)) * sizey;
	HZnum = sizeof(tfont24) / sizeof(typFNT_UTF8_24); // 统计汉字数目
	for (k = 0; k < HZnum; k++)
	{
		if ((tfont24[k].Index[0] == *(s)) && (tfont24[k].Index[1] == *(s + 1)) && (tfont24[k].Index[2] == *(s + 2)))
		{
			LCD_Address_Set(x, y, x + sizey - 1, y + sizey - 1);
			for (i = 0; i < TypefaceNum; i++)
			{
				for (j = 0; j < 8; j++)
				{
					if (!mode) // 非叠加方式
					{
						if (tfont24[k].Msk[i] & (0x01 << j))
							LCD_WR_DATA(fc);
						else
							LCD_WR_DATA(bc);
						m++;
						if (m % sizey == 0)
						{
							m = 0;
							break;
						}
					}
					else // 叠加方式
					{
						if (tfont24[k].Msk[i] & (0x01 << j))
							LCD_DrawPoint(x, y, fc); // 画一个点
						x++;
						if ((x - x0) == sizey)
						{
							x = x0;
							y++;
							break;
						}
					}
				}
			}
		}
		continue; // 查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
	}
}
/**
 * @brief 	显示单个32x32汉字
 * @param	x 列的坐标
 * @param	y 行的坐标
 * @param	s 要显示的汉字
 * @param	fc 字的颜色
 * @param	bc 字的背景色
 * @param	sizey 字号
 * @param	mode 0非叠加模式  1叠加模式
 * @return	无
 */
void LCD_ShowChinese32x32(uint16_t x, uint16_t y, uint8_t *s, uint16_t fc, uint16_t bc, uint8_t sizey, uint8_t mode)
{
	uint8_t i, j, m = 0;
	uint16_t k;
	uint16_t HZnum;		  // 汉字数目
	uint16_t TypefaceNum; // 一个字符所占字节大小
	uint16_t x0 = x;
	TypefaceNum = (sizey / 8 + ((sizey % 8) ? 1 : 0)) * sizey;
	HZnum = sizeof(tfont32) / sizeof(typFNT_UTF8_32); // 统计汉字数目
	for (k = 0; k < HZnum; k++)
	{
		if ((tfont32[k].Index[0] == *(s)) && (tfont32[k].Index[1] == *(s + 1)) && (tfont32[k].Index[2] == *(s + 2)))
		{
			LCD_Address_Set(x, y, x + sizey - 1, y + sizey - 1);
			for (i = 0; i < TypefaceNum; i++)
			{
				for (j = 0; j < 8; j++)
				{
					if (!mode) // 非叠加方式
					{
						if (tfont32[k].Msk[i] & (0x01 << j))
							LCD_WR_DATA(fc);
						else
							LCD_WR_DATA(bc);
						m++;
						if (m % sizey == 0)
						{
							m = 0;
							break;
						}
					}
					else // 叠加方式
					{
						if (tfont32[k].Msk[i] & (0x01 << j))
							LCD_DrawPoint(x, y, fc); // 画一个点
						x++;
						if ((x - x0) == sizey)
						{
							x = x0;
							y++;
							break;
						}
					}
				}
			}
		}
		continue; // 查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
	}
}

/**
 * @brief 	显示单个字符
 * @param	x 列的坐标
 * @param	y 行的坐标
 * @param	num 要显示的字符
 * @param	fc 字的颜色
 * @param	bc 字的背景色
 * @param	sizey 字号
 * @param	mode 0非叠加模式  1叠加模式
 * @return	无
 */
void LCD_ShowChar(uint16_t x, uint16_t y, uint8_t num, uint16_t fc, uint16_t bc, uint8_t sizey, uint8_t mode)
{
	uint8_t temp, sizex, t, m = 0;
	uint16_t i, TypefaceNum; // 一个字符所占字节大小
	uint16_t x0 = x;
	sizex = sizey / 2;
	TypefaceNum = (sizex / 8 + ((sizex % 8) ? 1 : 0)) * sizey;
	num = num - ' ';									 // 得到偏移后的值
	LCD_Address_Set(x, y, x + sizex - 1, y + sizey - 1); // 设置光标位置
	for (i = 0; i < TypefaceNum; i++)
	{
		if (sizey == 12)
			temp = ascii_1206[num][i]; // 调用6x12字体
		else if (sizey == 16)
			temp = ascii_1608[num][i]; // 调用8x16字体
		else if (sizey == 24)
			temp = ascii_2412[num][i]; // 调用12x24字体
		else if (sizey == 32)
			temp = ascii_3216[num][i]; // 调用16x32字体
		else
			return;
		for (t = 0; t < 8; t++)
		{
			if (!mode) // 非叠加模式
			{
				if (temp & (0x01 << t))
					LCD_WR_DATA(fc);
				else
					LCD_WR_DATA(bc);
				m++;
				if (m % sizex == 0)
				{
					m = 0;
					break;
				}
			}
			else // 叠加模式
			{
				if (temp & (0x01 << t))
					LCD_DrawPoint(x, y, fc); // 画一个点
				x++;
				if ((x - x0) == sizex)
				{
					x = x0;
					y++;
					break;
				}
			}
		}
	}
}

/**
 * @brief 	显示字符串
 * @param	x 列的坐标
 * @param	y 行的坐标
 * @param	p 要显示的字符串
 * @param	fc 字的颜色
 * @param	bc 字的背景色
 * @param	sizey 字号
 * @param	mode 0非叠加模式  1叠加模式
 * @return	无
 */
void LCD_ShowString(uint16_t x, uint16_t y, const uint8_t *p, uint16_t fc, uint16_t bc, uint8_t sizey, uint8_t mode)
{
	while (*p != '\0')
	{
		LCD_ShowChar(x, y, *p, fc, bc, sizey, mode);
		x += sizey / 2;
		p++;
	}
}

/**
 * @brief 	计算幂
 * @param	m 底数
 * @param	n 指数
 * @return	m^n
 */
u32 mypow(uint8_t m, uint8_t n)
{
	u32 result = 1;
	while (n--)
		result *= m;
	return result;
}

/**
 * @brief 	显示整数变量
 * @param	x 列的坐标
 * @param	y 行的坐标
 * @param	num 要显示整数变量
 * @param	len 要显示的位数
 * @param	fc 字的颜色
 * @param	bc 字的背景色
 * @param	sizey 字号
 * @return	无
 */
void LCD_ShowIntNum(uint16_t x, uint16_t y, uint16_t num, uint8_t len, uint16_t fc, uint16_t bc, uint8_t sizey)
{
	uint8_t t, temp;
	uint8_t enshow = 0;
	uint8_t sizex = sizey / 2;
	for (t = 0; t < len; t++)
	{
		temp = (num / mypow(10, len - t - 1)) % 10;
		if (enshow == 0 && t < (len - 1))
		{
			if (temp == 0)
			{
				LCD_ShowChar(x + t * sizex, y, ' ', fc, bc, sizey, 0);
				continue;
			}
			else
				enshow = 1;
		}
		LCD_ShowChar(x + t * sizex, y, temp + 48, fc, bc, sizey, 0);
	}
}

/**
 * @brief 	显示两位小数变量
 * @param	x 列的坐标
 * @param	y 行的坐标
 * @param	num 要显示小数变量
 * @param	len 要显示的位数
 * @param	fc 字的颜色
 * @param	bc 字的背景色
 * @param	sizey 字号
 * @return	无
 */
void LCD_ShowFloatNum1(uint16_t x, uint16_t y, float num, uint8_t len, uint16_t fc, uint16_t bc, uint8_t sizey)
{
	uint8_t t, temp, sizex;
	uint16_t num1;
	sizex = sizey / 2;
	num1 = num * 100;
	for (t = 0; t < len; t++)
	{
		temp = (num1 / mypow(10, len - t - 1)) % 10;
		if (t == (len - 2))
		{
			LCD_ShowChar(x + (len - 2) * sizex, y, '.', fc, bc, sizey, 0);
			t++;
			len += 1;
		}
		LCD_ShowChar(x + t * sizex, y, temp + 48, fc, bc, sizey, 0);
	}
}

/**
 * @brief 	显示图片
 * @param	x 列的坐标
 * @param	y 行的坐标
 * @param	length 图片长度
 * @param	width 图片宽度
 * @param	pic 图片数组
 * @return	无
 */
void LCD_ShowPicture(uint16_t x, uint16_t y, uint16_t length, uint16_t width, const uint8_t pic[])
{
	uint16_t i, j;
	u32 k = 0;
	LCD_Address_Set(x, y, x + length - 1, y + width - 1);
	for (i = 0; i < length; i++)
	{
		for (j = 0; j < width; j++)
		{
			LCD_WR_DATA8(pic[k * 2]);
			LCD_WR_DATA8(pic[k * 2 + 1]);
			k++;
		}
	}
}
