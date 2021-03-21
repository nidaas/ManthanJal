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
#include "lcd.h"
#include "stdlib.h"

	   
//����LCD��Ҫ����
//Ĭ��Ϊ����
_lcd_dev lcddev;

//������ɫ,������ɫ
u16 POINT_COLOR = 0x0000,BACK_COLOR = 0xFFFF;  
u16 DeviceCode;	 

void LCD_write(u16 VAL)
{
	//LCD_CS_CLR;
	DATAOUT(VAL);
	LCD_WR_CLR;
	LCD_WR_SET;
	//LCD_CS_SET;
}

/*****************************************************************************
 * @name       :void LCD_WR_REG(u16 data)
 * @date       :2018-08-09 
 * @function   :Write an 16-bit command to the LCD screen
 * @parameters :data:Command value to be written
 * @retvalue   :None
******************************************************************************/
void LCD_WR_REG(u16 data)
{ 
   LCD_RS_CLR;     
	 #if LCD_USE8BIT_MODEL
	 LCD_write(data<<8);
	 #else
	 LCD_write(data);
	 #endif
}

/*****************************************************************************
 * @name       :void LCD_WR_DATA(u16 data)
 * @date       :2018-08-09 
 * @function   :Write an 16-bit data to the LCD screen
 * @parameters :data:data value to be written
 * @retvalue   :None
******************************************************************************/
void LCD_WR_DATA(u16 data)
{
	 LCD_RS_SET;
	 #if LCD_USE8BIT_MODEL
	 LCD_write(data<<8);
	 #else
	 LCD_write(data);
	 #endif
}

/*****************************************************************************
 * @name       :void LCD_WriteReg(u16 LCD_Reg, u16 LCD_RegValue)
 * @date       :2018-08-09 
 * @function   :Write data into registers
 * @parameters :LCD_Reg:Register address
                LCD_RegValue:Data to be written
 * @retvalue   :None
******************************************************************************/
void LCD_WriteReg(u16 LCD_Reg, u16 LCD_RegValue)
{	
	LCD_WR_REG(LCD_Reg);  
	LCD_WR_DATA(LCD_RegValue);	    		 
}	   

/*****************************************************************************
 * @name       :void LCD_WriteRAM_Prepare(void)
 * @date       :2018-08-09 
 * @function   :Write GRAM
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	 
void LCD_WriteRAM_Prepare(void)
{
	LCD_WR_REG(lcddev.wramcmd);
}	 

/*****************************************************************************
 * @name       :void Lcd_WriteData_16Bit(u16 Data)
 * @date       :2018-08-09 
 * @function   :Write an 16-bit command to the LCD screen
 * @parameters :Data:Data to be written
 * @retvalue   :None
******************************************************************************/	 
void Lcd_WriteData_16Bit(u16 Data)
{	
   LCD_RS_SET; 
	 #if LCD_USE8BIT_MODEL
		LCD_CS_CLR;
		DATAOUT(Data&0xFF00);
		LCD_WR_CLR; 
		LCD_WR_SET;
		DATAOUT(Data<<8);
		LCD_WR_CLR; 
		LCD_WR_SET;
		LCD_CS_SET;
 //  LCD_write(Data&0xFF00);
//	 LCD_write(Data<<8);
	 #else
	 LCD_write(Data);
	 #endif
}

/*****************************************************************************
 * @name       :void LCD_DrawPoint(u16 x,u16 y)
 * @date       :2018-08-09 
 * @function   :Write a pixel data at a specified location
 * @parameters :x:the x coordinate of the pixel
                y:the y coordinate of the pixel
 * @retvalue   :None
******************************************************************************/	
void LCD_DrawPoint(u16 x,u16 y)
{
	LCD_SetCursor(x,y);//���ù��λ�� 
	Lcd_WriteData_16Bit(BACK_COLOR);
}

/*****************************************************************************
 * @name       :void LCD_Clear(u16 Color)
 * @date       :2018-08-09 
 * @function   :Full screen filled LCD screen
 * @parameters :color:Filled color
 * @retvalue   :None
******************************************************************************/	
void LCD_Clear(u16 Color)
{
  unsigned int i;//,m;  
  LCD_CS_CLR;
	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);   
	for(i=0;i<lcddev.height*lcddev.width;i++)
	{
 //   for(m=0;m<lcddev.width;m++)
  //  {	
			Lcd_WriteData_16Bit(Color);
	//	}
	}
} 

/*****************************************************************************
 * @name       :void LCD_GPIOInit(void)
 * @date       :2018-08-09 
 * @function   :Initialization LCD screen GPIO
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	
void LCD_GPIOInit(void)
{
//	GPIO_InitTypeDef  GPIO_InitStructure;
//
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOF|RCC_APB2Periph_AFIO,ENABLE);
//	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);
//
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10| GPIO_Pin_7| GPIO_Pin_8| GPIO_Pin_9; //GPIOC10,7,8,9
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   //�������
//	GPIO_Init(GPIOC, &GPIO_InitStructure);
//	GPIO_SetBits(GPIOC,GPIO_Pin_10|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9);
//
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;	//
//	GPIO_Init(GPIOF, &GPIO_InitStructure); //GPIOB
//	GPIO_SetBits(GPIOF,GPIO_Pin_All);
}

/*****************************************************************************
 * @name       :void LCD_RESET(void)
 * @date       :2018-08-09 
 * @function   :Reset LCD screen
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	
void LCD_RESET(void)
{
	LCD_RST_CLR;
	delay_ms(100);	
	LCD_RST_SET;
	delay_ms(50);
}

/*****************************************************************************
 * @name       :void LCD_Init(void)
 * @date       :2018-08-09 
 * @function   :Initialization LCD screen
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	 	 
void LCD_Init(void)
{  
	//LCD_GPIOInit();//LCD GPIO��ʼ��
 	LCD_RESET(); //LCD ��λ
//************* ILI9486��ʼ��**********//	
	LCD_WR_REG(0XF1);
	LCD_WR_DATA(0x36);
	LCD_WR_DATA(0x04);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x3C);
	LCD_WR_DATA(0X0F);
	LCD_WR_DATA(0x8F);
	LCD_WR_REG(0XF2);
	LCD_WR_DATA(0x18);
	LCD_WR_DATA(0xA3);
	LCD_WR_DATA(0x12);
	LCD_WR_DATA(0x02);
	LCD_WR_DATA(0XB2);
	LCD_WR_DATA(0x12);
	LCD_WR_DATA(0xFF);
	LCD_WR_DATA(0x10);
	LCD_WR_DATA(0x00);
	LCD_WR_REG(0XF8);
	LCD_WR_DATA(0x21);
	LCD_WR_DATA(0x04);
	LCD_WR_REG(0XF9);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x08);
	LCD_WR_REG(0x36);
	LCD_WR_DATA(0x08);
	LCD_WR_REG(0xB4);
	LCD_WR_DATA(0x00);
	LCD_WR_REG(0xC1);
	LCD_WR_DATA(0x41);
	LCD_WR_REG(0xC5);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x91);
	LCD_WR_DATA(0x80);
	LCD_WR_DATA(0x00);
	LCD_WR_REG(0xE0);
	LCD_WR_DATA(0x0F);
	LCD_WR_DATA(0x1F);
	LCD_WR_DATA(0x1C);
	LCD_WR_DATA(0x0C);
	LCD_WR_DATA(0x0F);
	LCD_WR_DATA(0x08);
	LCD_WR_DATA(0x48);
	LCD_WR_DATA(0x98);
	LCD_WR_DATA(0x37);
	LCD_WR_DATA(0x0A);
	LCD_WR_DATA(0x13);
	LCD_WR_DATA(0x04);
	LCD_WR_DATA(0x11);
	LCD_WR_DATA(0x0D);
	LCD_WR_DATA(0x00);
	LCD_WR_REG(0xE1);
	LCD_WR_DATA(0x0F);
	LCD_WR_DATA(0x32);
	LCD_WR_DATA(0x2E);
	LCD_WR_DATA(0x0B);
	LCD_WR_DATA(0x0D);
	LCD_WR_DATA(0x05);
	LCD_WR_DATA(0x47);
	LCD_WR_DATA(0x75);
	LCD_WR_DATA(0x37);
	LCD_WR_DATA(0x06);
	LCD_WR_DATA(0x10);
	LCD_WR_DATA(0x03);
	LCD_WR_DATA(0x24);
	LCD_WR_DATA(0x20);
	LCD_WR_DATA(0x00);
	LCD_WR_REG(0x3A);
	LCD_WR_DATA(0x55);
	LCD_WR_REG(0x11);
	LCD_WR_REG(0x36);
	LCD_WR_DATA(0x28);
	delay_ms(120);
	LCD_WR_REG(0x29);

	LCD_direction(USE_HORIZONTAL);//����LCD��ʾ����
//	LCD_LED=1;//��������	 
	LCD_Clear(GRAY);//��ȫ����ɫ
}
 
/*****************************************************************************
 * @name       :void LCD_SetWindows(u16 xStar, u16 yStar,u16 xEnd,u16 yEnd)
 * @date       :2018-08-09 
 * @function   :Setting LCD display window
 * @parameters :xStar:the bebinning x coordinate of the LCD display window
								yStar:the bebinning y coordinate of the LCD display window
								xEnd:the endning x coordinate of the LCD display window
								yEnd:the endning y coordinate of the LCD display window
 * @retvalue   :None
******************************************************************************/ 
void LCD_SetWindows(u16 xStar, u16 yStar,u16 xEnd,u16 yEnd)
{	
	LCD_WR_REG(lcddev.setxcmd);	
	LCD_WR_DATA(xStar>>8);
	LCD_WR_DATA(0x00FF&xStar);		
	LCD_WR_DATA(xEnd>>8);
	LCD_WR_DATA(0x00FF&xEnd);

	LCD_WR_REG(lcddev.setycmd);	
	LCD_WR_DATA(yStar>>8);
	LCD_WR_DATA(0x00FF&yStar);		
	LCD_WR_DATA(yEnd>>8);
	LCD_WR_DATA(0x00FF&yEnd);

	LCD_WriteRAM_Prepare();	//��ʼд��GRAM			
}   

/*****************************************************************************
 * @name       :void LCD_SetCursor(u16 Xpos, u16 Ypos)
 * @date       :2018-08-09 
 * @function   :Set coordinate value
 * @parameters :Xpos:the  x coordinate of the pixel
								Ypos:the  y coordinate of the pixel
 * @retvalue   :None
******************************************************************************/ 
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{	  	    			
	LCD_SetWindows(Xpos,Ypos,Xpos,Ypos);	
} 

/*****************************************************************************
 * @name       :void LCD_direction(u8 direction)
 * @date       :2018-08-09 
 * @function   :Setting the display direction of LCD screen
 * @parameters :direction:0-0 degree
                          1-90 degree
													2-180 degree
													3-270 degree
 * @retvalue   :None
******************************************************************************/ 
void LCD_direction(u8 direction)
{ 
			lcddev.setxcmd=0x2A;
			lcddev.setycmd=0x2B;
			lcddev.wramcmd=0x2C;
	switch(direction){		  
		case 0:						 	 		
			lcddev.width=LCD_W;
			lcddev.height=LCD_H;		
			LCD_WriteReg(0x36,(1<<6)|(1<<3));//0 degree MY=0,MX=0,MV=0,ML=0,BGR=1,MH=0
		break;
		case 1:
			lcddev.width=LCD_H;
			lcddev.height=LCD_W;
			LCD_WriteReg(0x36,(1<<3)|(1<<4)|(1<<5));//90 degree MY=0,MX=1,MV=1,ML=1,BGR=1,MH=0
		break;
		case 2:						 	 		
			lcddev.width=LCD_W;
			lcddev.height=LCD_H;	
			LCD_WriteReg(0x36,(1<<3)|(1<<7));//180 degree MY=1,MX=1,MV=0,ML=0,BGR=1,MH=0
		break;
		case 3:
			lcddev.width=LCD_H;
			lcddev.height=LCD_W;
			LCD_WriteReg(0x36,(1<<3)|(1<<5)|(1<<6)|(1<<7));//270 degree MY=1,MX=0,MV=1,ML=0,BGR=1,MH=0
		break;	
		default:break;
	}		
}	 
