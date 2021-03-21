//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//����Ӳ������Ƭ��STM32F103ZET6,����ԭ��ELITE STM32������,��Ƶ72MHZ������12MHZ
//QDtech-TFTҺ������ for STM32 IOģ��
//xiao��@ShenZhen QDtech co.,LTD
//��˾��վ:www.qdtft.com
//�Ա���վ��http://qdtech.taobao.com
//wiki������վ��http://www.lcdwiki.com
//��˾�ṩ����֧�֣��κμ������⻶ӭ��ʱ����ѧϰ
//�̻�(����) :+86 0755-23594567 
//�ֻ�:15989313508���빤�� 
//����:lcdwiki01@gmail.com    support@lcdwiki.com    goodtft@163.com 
//����֧��QQ:3002773612  3002778157
//��������QQȺ:324828016
//��������:2018/08/09
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������ȫ�����Ӽ������޹�˾ 2018-2028
//All rights reserved
/****************************************************************************************************
//=========================================��Դ����================================================//
//     LCDģ��                STM32��Ƭ��
//      VCC          ��        DC5V/3.3V        //��Դ
//      GND          ��          GND            //��Դ��
//=======================================Һ���������߽���==========================================//
//��ģ��Ĭ��������������ΪSPI����
//     LCDģ��                STM32��Ƭ��    
//     DB0~DB15      ��        PF0~PF15         //Һ����16λ���������ź�
//=======================================Һ���������߽���==========================================//
//     LCDģ�� 				  STM32��Ƭ�� 
//     LCD_WR        ��          PC7           //Һ����д���ݿ����ź�
//     LCD_RS        ��          PC8           //Һ��������/��������ź�
//     LCD_RST       ��          PC10          //Һ������λ�����ź�
//     LCD_CS        ��          PC9           //Һ����Ƭѡ�����ź�
//=========================================������������=========================================//
//���ģ�鲻���������ܻ��ߴ��д������ܣ����ǲ���Ҫ�������ܣ�����Ҫ���д���������
//	   LCDģ��                STM32��Ƭ�� 
//      T_IRQ        ��          PC1        //�����������ж��ź�
//      T_DO         ��          PC2          //������SPI���߶��ź�
//      T_DIN        ��          PC3          //������SPI����д�ź�
//      T_CS         ��          PC13         //������Ƭѡ�����ź�
//      T_CLK        ��          PC0          //������SPI����ʱ���ź�
**************************************************************************************************/	
 /* @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, QD electronic SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
**************************************************************************************************/			
#ifndef __GUI_H__
#define __GUI_H__
#include <stdbool.h>
#include <stdint.h>
#include "lcd.h"
#define LEFT 0
#define RIGHT 9999
#define CENTER 9998

void GUI_DrawPoint(u16 x,u16 y,u16 color);
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color);
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2);
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2);
void Draw_Circle1(u16 x0,u16 y0,u16 fc,u8 r);
void Draw_Triangel(u16 x0,u16 y0,u16 x1,u16 y1,u16 x2,u16 y2);
void Fill_Triangel(u16 x0,u16 y0,u16 x1,u16 y1,u16 x2,u16 y2);
void LCD_ShowChar(u16 x,u16 y,u16 fc, u16 bc, u8 num,u8 size,u8 mode);
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size);
void LCD_Show2Num(u16 x,u16 y,u16 num,u8 len,u8 size,u8 mode);
void LCD_ShowString(u16 x,u16 y,u8 size,u8 *p,u8 mode);
void GUI_DrawFont16(u16 x, u16 y, u16 fc, u16 bc, u8 *s,u8 mode);
void GUI_DrawFont24(u16 x, u16 y, u16 fc, u16 bc, u8 *s,u8 mode);
void GUI_DrawFont32(u16 x, u16 y, u16 fc, u16 bc, u8 *s,u8 mode);
void Show_Str(u16 x, u16 y, u16 fc, u16 bc, u8 *str,u8 size,u8 mode);
void Gui_Drawbmp16(u16 x,u16 y,const unsigned char *p); //��ʾ40*40 QQͼƬ
void gui_circle(int xc, int yc,u16 c,int r, int fill);
void Gui_StrCenter(u16 x, u16 y, u16 fc, u16 bc, u8 *str,u8 size,u8 mode);
void LCD_DrawFillRectangle(u16 x1, u16 y1, u16 x2, u16 y2);
void LCD_DrawFillRectangleColor(u16 x1, u16 y1, u16 x2, u16 y2,uint16_t color);


void Draw_Round_Rectangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t radius);
void Fill_Round_Rectangle(int16_t x1, int16_t y1, int16_t x2,int16_t y2, int16_t radius,u16 color);
void Draw_Circle(int16_t x, int16_t y, int16_t radius);
void Draw_Circle_Helper(int16_t x0, int16_t y0, int16_t radius, uint8_t cornername);
void Fill_Circle(int16_t x, int16_t y, int16_t radius);
void Fill_Circle_Helper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername,int16_t delta);
void Draw_Fast_VLine(int16_t x, int16_t y, int16_t h);
void Draw_Fast_HLine(int16_t x, int16_t y, int16_t w);
void Fill_Rect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void Draw_Pixe(int16_t x, int16_t y, uint16_t color);


void Set_Text_Cousur(int16_t x, int16_t y);
int16_t Get_Text_X_Cousur(void) ;
int16_t Get_Text_Y_Cousur(void) ;
void Set_Text_colour(uint16_t color);
uint16_t Get_Text_colour(void) ;
void Set_Text_Back_colour(uint16_t color);
uint16_t Get_Text_Back_colour(void);
void Set_Text_Size(uint8_t s);
uint8_t Get_Text_Size(void);
void Set_Text_Mode(bool mode);
bool Get_Text_Mode(void);
size_t lcd_Print(uint8_t *st, int16_t x, int16_t y);
void lcd_Print_String(uint8_t *st, int16_t x, int16_t y);
void lcd_Print_Number_Int(long num, int16_t x, int16_t y, int16_t length, uint8_t filler, int16_t system);
void lcd_Print_Number_Float(double num, uint8_t dec, int16_t x, int16_t y, uint8_t divider, int16_t length, uint8_t filler);
void Draw_Char(int16_t x, int16_t y, uint8_t c, uint16_t color,uint16_t bg, uint8_t size, bool mode);
size_t lcd_write(uint8_t c);
int16_t Get_Display_Width(void) ;
int16_t Get_Display_Height(void);
void LCD_ShowpH7(u16 x,u16 y,u16 fc, u16 bc,u8 size,u8 mode);
#endif

