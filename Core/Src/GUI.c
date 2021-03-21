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
#include "string.h"
#include "font.h"
#include "gui.h"

u16 draw_color=0;
int16_t text_x, text_y;
uint16_t text_color, text_bgcolor,draw_color;
uint8_t text_size;
bool text_mode; //if set,text_bgcolor is invalid

/*******************************************************************
 * @name       :void GUI_DrawPoint(u16 x,u16 y,u16 color)
 * @date       :2018-08-09 
 * @function   :draw a point in LCD screen
 * @parameters :x:the x coordinate of the point
                y:the y coordinate of the point
								color:the color value of the point
 * @retvalue   :None
********************************************************************/
void GUI_DrawPoint(u16 x,u16 y,u16 color)
{
	LCD_SetCursor(x,y);//���ù��λ�� 
	Lcd_WriteData_16Bit(color); 
}

/*******************************************************************
 * @name       :void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color)
 * @date       :2018-08-09 
 * @function   :fill the specified area
 * @parameters :sx:the bebinning x coordinate of the specified area
                sy:the bebinning y coordinate of the specified area
								ex:the ending x coordinate of the specified area
								ey:the ending y coordinate of the specified area
								color:the filled color value
 * @retvalue   :None
********************************************************************/
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color)
{  	
	u16 i,j;			
	u16 width=ex-sx+1; 		//�õ����Ŀ��
	u16 height=ey-sy+1;		//�߶�
	LCD_SetWindows(sx,sy,ex,ey);//������ʾ����
	for(i=0;i<height;i++)
	{
		for(j=0;j<width;j++)
		Lcd_WriteData_16Bit(color);	//д������ 	 
	}
	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);//�ָ���������Ϊȫ��
}

/*******************************************************************
 * @name       :void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
 * @date       :2018-08-09 
 * @function   :Draw a line between two points
 * @parameters :x1:the bebinning x coordinate of the line
                y1:the bebinning y coordinate of the line
								x2:the ending x coordinate of the line
								y2:the ending y coordinate of the line
 * @retvalue   :None
********************************************************************/
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 

	delta_x=x2-x1; //������������ 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //���õ������� 
	else if(delta_x==0)incx=0;//��ֱ�� 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//ˮƽ�� 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //ѡȡ�������������� 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//������� 
	{  
		LCD_DrawPoint(uRow,uCol);//���� 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
} 

/*****************************************************************************
 * @name       :void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
 * @date       :2018-08-09 
 * @function   :Draw a rectangle
 * @parameters :x1:the bebinning x coordinate of the rectangle
                y1:the bebinning y coordinate of the rectangle
								x2:the ending x coordinate of the rectangle
								y2:the ending y coordinate of the rectangle
 * @retvalue   :None
******************************************************************************/
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
{
	LCD_DrawLine(x1,y1,x2,y1);
	LCD_DrawLine(x1,y1,x1,y2);
	LCD_DrawLine(x1,y2,x2,y2);
	LCD_DrawLine(x2,y1,x2,y2);
}  

/*****************************************************************************
 * @name       :void LCD_DrawFillRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
 * @date       :2018-08-09 
 * @function   :Filled a rectangle
 * @parameters :x1:the bebinning x coordinate of the filled rectangle
                y1:the bebinning y coordinate of the filled rectangle
								x2:the ending x coordinate of the filled rectangle
								y2:the ending y coordinate of the filled rectangle
 * @retvalue   :None
******************************************************************************/  
void LCD_DrawFillRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
{
	LCD_Fill(x1,y1,x2,y2,BACK_COLOR);
}


/*****************************************************************************
 * @name       :void LCD_DrawFillRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
 * @date       :2018-08-09
 * @function   :Filled a rectangle
 * @parameters :x1:the bebinning x coordinate of the filled rectangle
                y1:the bebinning y coordinate of the filled rectangle
								x2:the ending x coordinate of the filled rectangle
								y2:the ending y coordinate of the filled rectangle
 * @retvalue   :None
******************************************************************************/
void LCD_DrawFillRectangleColor(u16 x1, u16 y1, u16 x2, u16 y2,uint16_t color)
{
	LCD_Fill(x1,y1,x2,y2,color);
}
 
/*****************************************************************************
 * @name       :void _draw_circle_8(int xc, int yc, int x, int y, u16 c)
 * @date       :2018-08-09 
 * @function   :8 symmetry circle drawing algorithm (internal call)
 * @parameters :xc:the x coordinate of the Circular center 
                yc:the y coordinate of the Circular center 
								x:the x coordinate relative to the Circular center 
								y:the y coordinate relative to the Circular center 
								c:the color value of the circle
 * @retvalue   :None
******************************************************************************/  
void _draw_circle_8(int xc, int yc, int x, int y, u16 c)
{
	GUI_DrawPoint(xc + x, yc + y, c);

	GUI_DrawPoint(xc - x, yc + y, c);

	GUI_DrawPoint(xc + x, yc - y, c);

	GUI_DrawPoint(xc - x, yc - y, c);

	GUI_DrawPoint(xc + y, yc + x, c);

	GUI_DrawPoint(xc - y, yc + x, c);

	GUI_DrawPoint(xc + y, yc - x, c);

	GUI_DrawPoint(xc - y, yc - x, c);
}

/*****************************************************************************
 * @name       :void gui_circle(int xc, int yc,u16 c,int r, int fill)
 * @date       :2018-08-09 
 * @function   :Draw a circle of specified size at a specified location
 * @parameters :xc:the x coordinate of the Circular center 
                yc:the y coordinate of the Circular center 
								r:Circular radius
								fill:1-filling,0-no filling
 * @retvalue   :None
******************************************************************************/  
void gui_circle(int xc, int yc,u16 c,int r, int fill)
{
	int x = 0, y = r, yi, d;

	d = 3 - 2 * r;


	if (fill) 
	{
		// �����䣨��ʵ��Բ��
		while (x <= y) {
			for (yi = x; yi <= y; yi++)
				_draw_circle_8(xc, yc, x, yi, c);

			if (d < 0) {
				d = d + 4 * x + 6;
			} else {
				d = d + 4 * (x - y) + 10;
				y--;
			}
			x++;
		}
	} else 
	{
		// �������䣨������Բ��
		while (x <= y) {
			_draw_circle_8(xc, yc, x, y, c);
			if (d < 0) {
				d = d + 4 * x + 6;
			} else {
				d = d + 4 * (x - y) + 10;
				y--;
			}
			x++;
		}
	}
}

/*****************************************************************************
 * @name       :void Draw_Triangel(u16 x0,u16 y0,u16 x1,u16 y1,u16 x2,u16 y2)
 * @date       :2018-08-09 
 * @function   :Draw a triangle at a specified position
 * @parameters :x0:the bebinning x coordinate of the triangular edge 
                y0:the bebinning y coordinate of the triangular edge 
								x1:the vertex x coordinate of the triangular
								y1:the vertex y coordinate of the triangular
								x2:the ending x coordinate of the triangular edge 
								y2:the ending y coordinate of the triangular edge 
 * @retvalue   :None
******************************************************************************/ 
void Draw_Triangel(u16 x0,u16 y0,u16 x1,u16 y1,u16 x2,u16 y2)
{
	LCD_DrawLine(x0,y0,x1,y1);
	LCD_DrawLine(x1,y1,x2,y2);
	LCD_DrawLine(x2,y2,x0,y0);
}

static void _swap(u16 *a, u16 *b)
{
	u16 tmp;
  tmp = *a;
	*a = *b;
	*b = tmp;
}

/*****************************************************************************
 * @name       :void Fill_Triangel(u16 x0,u16 y0,u16 x1,u16 y1,u16 x2,u16 y2)
 * @date       :2018-08-09 
 * @function   :filling a triangle at a specified position
 * @parameters :x0:the bebinning x coordinate of the triangular edge 
                y0:the bebinning y coordinate of the triangular edge 
								x1:the vertex x coordinate of the triangular
								y1:the vertex y coordinate of the triangular
								x2:the ending x coordinate of the triangular edge 
								y2:the ending y coordinate of the triangular edge 
 * @retvalue   :None
******************************************************************************/ 
void Fill_Triangel(u16 x0,u16 y0,u16 x1,u16 y1,u16 x2,u16 y2)
{
	u16 a, b, y, last;
	int dx01, dy01, dx02, dy02, dx12, dy12;
	long sa = 0;
	long sb = 0;
 	if (y0 > y1) 
	{
    _swap(&y0,&y1); 
		_swap(&x0,&x1);
 	}
 	if (y1 > y2) 
	{
    _swap(&y2,&y1); 
		_swap(&x2,&x1);
 	}
  if (y0 > y1) 
	{
    _swap(&y0,&y1); 
		_swap(&x0,&x1);
  }
	if(y0 == y2) 
	{ 
		a = b = x0;
		if(x1 < a)
    {
			a = x1;
    }
    else if(x1 > b)
    {
			b = x1;
    }
    if(x2 < a)
    {
			a = x2;
    }
		else if(x2 > b)
    {
			b = x2;
    }
		LCD_Fill(a,y0,b,y0,POINT_COLOR);
    return;
	}
	dx01 = x1 - x0;
	dy01 = y1 - y0;
	dx02 = x2 - x0;
	dy02 = y2 - y0;
	dx12 = x2 - x1;
	dy12 = y2 - y1;
	
	if(y1 == y2)
	{
		last = y1; 
	}
  else
	{
		last = y1-1; 
	}
	for(y=y0; y<=last; y++) 
	{
		a = x0 + sa / dy01;
		b = x0 + sb / dy02;
		sa += dx01;
    sb += dx02;
    if(a > b)
    {
			_swap(&a,&b);
		}
		LCD_Fill(a,y,b,y,POINT_COLOR);
	}
	sa = dx12 * (y - y1);
	sb = dx02 * (y - y0);
	for(; y<=y2; y++) 
	{
		a = x1 + sa / dy12;
		b = x0 + sb / dy02;
		sa += dx12;
		sb += dx02;
		if(a > b)
		{
			_swap(&a,&b);
		}
		LCD_Fill(a,y,b,y,POINT_COLOR);
	}
}

/*****************************************************************************
 * @name       :void LCD_ShowChar(u16 x,u16 y,u16 fc, u16 bc, u8 num,u8 size,u8 mode)
 * @date       :2018-08-09 
 * @function   :Display a single English character
 * @parameters :x:the bebinning x coordinate of the Character display position
                y:the bebinning y coordinate of the Character display position
								fc:the color value of display character
								bc:the background color of display character
								num:the ascii code of display character(0~94)
								size:the size of display character
								mode:0-no overlying,1-overlying
 * @retvalue   :None
******************************************************************************/ 
void LCD_ShowChar(u16 x,u16 y,u16 fc, u16 bc, u8 num,u8 size,u8 mode)
{  
    u8 temp;
    u8 pos,t;
	u16 colortemp=POINT_COLOR;      
		   
	num=num-' ';//�õ�ƫ�ƺ��ֵ
	LCD_SetWindows(x,y,x+size/2-1,y+size-1);//���õ���������ʾ����
	if(!mode) //�ǵ��ӷ�ʽ
	{		
		for(pos=0;pos<size;pos++)
		{
			if(size==12)temp=asc2_1206[num][pos];//����1206����
			else temp=asc2_1608[num][pos];		 //����1608����
			for(t=0;t<size/2;t++)
		    {                 
		        if(temp&0x01)Lcd_WriteData_16Bit(fc); 
				else Lcd_WriteData_16Bit(bc); 
				temp>>=1; 
				
		    }
			
		}	
	}else//���ӷ�ʽ
	{
		for(pos=0;pos<size;pos++)
		{
			if(size==12)temp=asc2_1206[num][pos];//����1206����
			else temp=asc2_1608[num][pos];		 //����1608����
			for(t=0;t<size/2;t++)
		    {   
				POINT_COLOR=fc;              
		        if(temp&0x01)LCD_DrawPoint(x+t,y+pos);//��һ����    
		        temp>>=1; 
		    }
		}
	}
	POINT_COLOR=colortemp;	
	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);//�ָ�����Ϊȫ��    	   	 	  
}

/*****************************************************************************
 * @name       :void LCD_ShowString(u16 x,u16 y,u8 size,u8 *p,u8 mode)
 * @date       :2018-08-09 
 * @function   :Display English string
 * @parameters :x:the bebinning x coordinate of the English string
                y:the bebinning y coordinate of the English string
								p:the start address of the English string
								size:the size of display character
								mode:0-no overlying,1-overlying
 * @retvalue   :None
******************************************************************************/   	  
void LCD_ShowString(u16 x,u16 y,u8 size,u8 *p,u8 mode)
{         
    while((*p<='~')&&(*p>=' '))//�ж��ǲ��ǷǷ��ַ�!
    {   
		if(x>(lcddev.width-1)||y>(lcddev.height-1)) 
		return;     
        LCD_ShowChar(x,y,POINT_COLOR,BACK_COLOR,*p,size,mode);
        x+=size/2;
        p++;
    }  
} 

/*****************************************************************************
 * @name       :u32 mypow(u8 m,u8 n)
 * @date       :2018-08-09 
 * @function   :get the nth power of m (internal call)
 * @parameters :m:the multiplier
                n:the power
 * @retvalue   :the nth power of m
******************************************************************************/ 
u32 mypow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}

/*****************************************************************************
 * @name       :void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size)
 * @date       :2018-08-09 
 * @function   :Display number
 * @parameters :x:the bebinning x coordinate of the number
                y:the bebinning y coordinate of the number
								num:the number(0~4294967295)
								len:the length of the display number
								size:the size of display number
 * @retvalue   :None
******************************************************************************/  			 
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+(size/2)*t,y,POINT_COLOR,BACK_COLOR,' ',size,0);
				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+(size/2)*t,y,POINT_COLOR,BACK_COLOR,temp+'0',size,0); 
	}
} 

/*****************************************************************************
 * @name       :void GUI_DrawFont16(u16 x, u16 y, u16 fc, u16 bc, u8 *s,u8 mode)
 * @date       :2018-08-09 
 * @function   :Display a single 16x16 Chinese character
 * @parameters :x:the bebinning x coordinate of the Chinese character
                y:the bebinning y coordinate of the Chinese character
								fc:the color value of Chinese character
								bc:the background color of Chinese character
								s:the start address of the Chinese character
								mode:0-no overlying,1-overlying
 * @retvalue   :None
******************************************************************************/ 
void GUI_DrawFont16(u16 x, u16 y, u16 fc, u16 bc, u8 *s,u8 mode)
{
	u8 i,j;
	u16 k;
	u16 HZnum;
	u16 x0=x;
	HZnum=sizeof(tfont16)/sizeof(typFNT_GB16);	//�Զ�ͳ�ƺ�����Ŀ
	
			
	for (k=0;k<HZnum;k++) 
	{
	  if ((tfont16[k].Index[0]==*(s))&&(tfont16[k].Index[1]==*(s+1)))
	  { 	LCD_SetWindows(x,y,x+16-1,y+16-1);
		    for(i=0;i<16*2;i++)
		    {
				for(j=0;j<8;j++)
		    	{	
					if(!mode) //�ǵ��ӷ�ʽ
					{
						if(tfont16[k].Msk[i]&(0x80>>j))	Lcd_WriteData_16Bit(fc);
						else Lcd_WriteData_16Bit(bc);
					}
					else
					{
						POINT_COLOR=fc;
						if(tfont16[k].Msk[i]&(0x80>>j))	LCD_DrawPoint(x,y);//��һ����
						x++;
						if((x-x0)==16)
						{
							x=x0;
							y++;
							break;
						}
					}

				}
				
			}
			
			
		}				  	
		continue;  //���ҵ���Ӧ�����ֿ������˳�����ֹ��������ظ�ȡģ����Ӱ��
	}

	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);//�ָ�����Ϊȫ��  
} 

/*****************************************************************************
 * @name       :void GUI_DrawFont24(u16 x, u16 y, u16 fc, u16 bc, u8 *s,u8 mode)
 * @date       :2018-08-09 
 * @function   :Display a single 24x24 Chinese character
 * @parameters :x:the bebinning x coordinate of the Chinese character
                y:the bebinning y coordinate of the Chinese character
								fc:the color value of Chinese character
								bc:the background color of Chinese character
								s:the start address of the Chinese character
								mode:0-no overlying,1-overlying
 * @retvalue   :None
******************************************************************************/ 
void GUI_DrawFont24(u16 x, u16 y, u16 fc, u16 bc, u8 *s,u8 mode)
{
	u8 i,j;
	u16 k;
	u16 HZnum;
	u16 x0=x;
	HZnum=sizeof(tfont24)/sizeof(typFNT_GB24);	//�Զ�ͳ�ƺ�����Ŀ
		
			for (k=0;k<HZnum;k++) 
			{
			  if ((tfont24[k].Index[0]==*(s))&&(tfont24[k].Index[1]==*(s+1)))
			  { 	LCD_SetWindows(x,y,x+24-1,y+24-1);
				    for(i=0;i<24*3;i++)
				    {
							for(j=0;j<8;j++)
							{
								if(!mode) //�ǵ��ӷ�ʽ
								{
									if(tfont24[k].Msk[i]&(0x80>>j))	Lcd_WriteData_16Bit(fc);
									else Lcd_WriteData_16Bit(bc);
								}
							else
							{
								POINT_COLOR=fc;
								if(tfont24[k].Msk[i]&(0x80>>j))	LCD_DrawPoint(x,y);//��һ����
								x++;
								if((x-x0)==24)
								{
									x=x0;
									y++;
									break;
								}
							}
						}
					}
					
					
				}				  	
				continue;  //���ҵ���Ӧ�����ֿ������˳�����ֹ��������ظ�ȡģ����Ӱ��
			}

	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);//�ָ�����Ϊȫ��  
}

/*****************************************************************************
 * @name       :void GUI_DrawFont32(u16 x, u16 y, u16 fc, u16 bc, u8 *s,u8 mode)
 * @date       :2018-08-09 
 * @function   :Display a single 32x32 Chinese character
 * @parameters :x:the bebinning x coordinate of the Chinese character
                y:the bebinning y coordinate of the Chinese character
								fc:the color value of Chinese character
								bc:the background color of Chinese character
								s:the start address of the Chinese character
								mode:0-no overlying,1-overlying
 * @retvalue   :None
******************************************************************************/ 
void GUI_DrawFont32(u16 x, u16 y, u16 fc, u16 bc, u8 *s,u8 mode)
{
	u8 i,j;
	u16 k;
	u16 HZnum;
	u16 x0=x;
	HZnum=sizeof(tfont32)/sizeof(typFNT_GB32);	//�Զ�ͳ�ƺ�����Ŀ
	for (k=0;k<HZnum;k++) 
			{
			  if ((tfont32[k].Index[0]==*(s))&&(tfont32[k].Index[1]==*(s+1)))
			  { 	LCD_SetWindows(x,y,x+32-1,y+32-1);
				    for(i=0;i<32*4;i++)
				    {
						for(j=0;j<8;j++)
				    	{
							if(!mode) //�ǵ��ӷ�ʽ
							{
								if(tfont32[k].Msk[i]&(0x80>>j))	Lcd_WriteData_16Bit(fc);
								else Lcd_WriteData_16Bit(bc);
							}
							else
							{
								POINT_COLOR=fc;
								if(tfont32[k].Msk[i]&(0x80>>j))	LCD_DrawPoint(x,y);//��һ����
								x++;
								if((x-x0)==32)
								{
									x=x0;
									y++;
									break;
								}
							}
						}
					}
					
					
				}				  	
				continue;  //���ҵ���Ӧ�����ֿ������˳�����ֹ��������ظ�ȡģ����Ӱ��
			}
	
	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);//�ָ�����Ϊȫ��  
} 

/*****************************************************************************
 * @name       :void Show_Str(u16 x, u16 y, u16 fc, u16 bc, u8 *str,u8 size,u8 mode)
 * @date       :2018-08-09 
 * @function   :Display Chinese and English strings
 * @parameters :x:the bebinning x coordinate of the Chinese and English strings
                y:the bebinning y coordinate of the Chinese and English strings
								fc:the color value of Chinese and English strings
								bc:the background color of Chinese and English strings
								str:the start address of the Chinese and English strings
								size:the size of Chinese and English strings
								mode:0-no overlying,1-overlying
 * @retvalue   :None
******************************************************************************/	   		   
void Show_Str(u16 x, u16 y, u16 fc, u16 bc, u8 *str,u8 size,u8 mode)
{					
	u16 x0=x;							  	  
  	u8 bHz=0;     //�ַ��������� 
    while(*str!=0)//����δ����
    { 
        if(!bHz)
        {
			if(x>(lcddev.width-size/2)||y>(lcddev.height-size)) 
			return; 
	        if(*str>0x80)bHz=1;//���� 
	        else              //�ַ�
	        {          
		        if(*str==0x0D)//���з���
		        {         
		            y+=size;
					x=x0;
		            str++; 
		        }  
		        else
				{
					if(size>16)//�ֿ���û�м���12X24 16X32��Ӣ������,��8X16����
					{  
					LCD_ShowChar(x,y,fc,bc,*str,16,mode);
					x+=8; //�ַ�,Ϊȫ�ֵ�һ�� 
					}
					else
					{
					LCD_ShowChar(x,y,fc,bc,*str,size,mode);
					x+=size/2; //�ַ�,Ϊȫ�ֵ�һ�� 
					}
				} 
				str++; 
		        
	        }
        }else//���� 
        {   
			if(x>(lcddev.width-size)||y>(lcddev.height-size)) 
			return;  
            bHz=0;//�к��ֿ�    
			if(size==32)
			GUI_DrawFont32(x,y,fc,bc,str,mode);	 	
			else if(size==24)
			GUI_DrawFont24(x,y,fc,bc,str,mode);	
			else
			GUI_DrawFont16(x,y,fc,bc,str,mode);
				
	        str+=2; 
	        x+=size;//��һ������ƫ��	    
        }						 
    }   
}

/*****************************************************************************
 * @name       :void Gui_StrCenter(u16 x, u16 y, u16 fc, u16 bc, u8 *str,u8 size,u8 mode)
 * @date       :2018-08-09 
 * @function   :Centered display of English and Chinese strings
 * @parameters :x:the bebinning x coordinate of the Chinese and English strings
                y:the bebinning y coordinate of the Chinese and English strings
								fc:the color value of Chinese and English strings
								bc:the background color of Chinese and English strings
								str:the start address of the Chinese and English strings
								size:the size of Chinese and English strings
								mode:0-no overlying,1-overlying
 * @retvalue   :None
******************************************************************************/ 
void Gui_StrCenter(u16 x, u16 y, u16 fc, u16 bc, u8 *str,u8 size,u8 mode)
{
	u16 len=strlen((const char *)str);
	u16 x1=(lcddev.width-len*8)/2;
	Show_Str(x1,y,fc,bc,str,size,mode);
} 
 
/*****************************************************************************
 * @name       :void Gui_Drawbmp16(u16 x,u16 y,const unsigned char *p)
 * @date       :2018-08-09 
 * @function   :Display a 16-bit BMP image
 * @parameters :x:the bebinning x coordinate of the BMP image
                y:the bebinning y coordinate of the BMP image
								p:the start address of image array
 * @retvalue   :None
******************************************************************************/ 
void Gui_Drawbmp16(u16 x,u16 y,const unsigned char *p) //��ʾ40*40 QQͼƬ
{
  	int i; 
	unsigned char picH,picL; 
	LCD_SetWindows(x,y,x+40-1,y+40-1);//��������
    for(i=0;i<40*40;i++)
	{	
	 	picL=*(p+i*2);	//���ݵ�λ��ǰ
		picH=*(p+i*2+1);				
		Lcd_WriteData_16Bit(picH<<8|picL);  						
	}	
	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);//�ָ���ʾ����Ϊȫ��	

}




//get lcd width
int16_t Get_Width(void)
{
	return lcddev.width;
}

//get lcd height
int16_t Get_Height(void)
{
	return lcddev.height;
}


//set x,y  coordinate and color to draw a pixel point
void Draw_Pixe(int16_t x, int16_t y, uint16_t color)
{
	if((x < 0) || (y < 0) || (x > Get_Width()) || (y > Get_Height()))
	{
		return;
	}
	LCD_SetWindows(x, y, x, y);
	Lcd_WriteData_16Bit(color);
}

//fill area from x to x+w,y to y+h
void Fill_Rect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
	int16_t end;
	if (w < 0)
	{
        w = -w;
        x -= w;
    }                           //+ve w
    end = x + w;
    if (x < 0)
    {
        x = 0;
    }
    if (end > Get_Width())
    {
        end = Get_Width();
    }
    w = end - x;
    if (h < 0)
	{
        h = -h;
        y -= h;
    }                           //+ve h
    end = y + h;
    if (y < 0)
    {
        y = 0;
    }
    if (end > Get_Height())
    {
        end = Get_Height();
    }
    h = end - y;
    LCD_SetWindows(x, y, x + w - 1, y + h - 1);//set area
	if (h > w)
	{
        end = h;
        h = w;
        w = end;
    }
	while (h-- > 0)
	{
		end = w;
		do
		{
			Lcd_WriteData_16Bit(color);//set color data
        } while (--end != 0);
	}
}



//set 16bits draw color
void Set_Draw_color(uint16_t color)
{
	draw_color = color;
}

//draw a pixel point
void Draw_Pixel(int16_t x, int16_t y)
{
	Draw_Pixe(x, y, draw_color);
}

//draw a round rectangle
void Draw_Round_Rectangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t radius)
{
	int w = x2 - x1 + 1, h = y2 - y1 + 1;
	if (w < 0)
	{
		x1 = x2;
		w = -w;
	}
 	if (h < 0)
	{
		y1 = y2;
		h = -h;
	}
	Draw_Fast_HLine(x1+radius, y1, w-2*radius);
	Draw_Fast_HLine(x1+radius, y1+h-1, w-2*radius);
	Draw_Fast_VLine(x1, y1+radius, h-2*radius);
  	Draw_Fast_VLine(x1+w-1, y1+radius, h-2*radius);
	Draw_Circle_Helper(x1+radius, y1+radius, radius, 1);
	Draw_Circle_Helper(x1+w-radius-1, y1+radius, radius, 2);
	Draw_Circle_Helper(x1+w-radius-1, y1+h-radius-1, radius, 4);
	Draw_Circle_Helper(x1+radius, y1+h-radius-1, radius, 8);
}

//fill a round rectangle
void Fill_Round_Rectangle(int16_t x1, int16_t y1, int16_t x2,int16_t y2, int16_t radius,u16 color)
{
	int w = x2 - x1 + 1, h = y2 - y1 + 1;
	if (w < 0)
	{
		x1 = x2;
		w = -w;
	}
	if (h < 0)
	{
		y1 = y2;
		h = -h;
	}
	draw_color=color;
	Fill_Rect(x1+radius, y1, w-2*radius, h,draw_color);
	Fill_Circle_Helper(x1+w-radius-1, y1+radius, radius, 1, h-2*radius-1);
	Fill_Circle_Helper(x1+radius, y1+radius, radius, 2, h-2*radius-1);
}

//draw a circle
void Draw_Circle(int16_t x, int16_t y, int16_t radius)
{
	int16_t f = 1 - radius;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * radius;
	int16_t x1= 0;
	int16_t y1= radius;

	Draw_Pixel(x, y+radius);
 	Draw_Pixel(x, y-radius);
	Draw_Pixel(x+radius, y);
	Draw_Pixel(x-radius, y);

	while (x1<y1)
	{
    	if (f >= 0)
		{
      		y1--;
      		ddF_y += 2;
      		f += ddF_y;
    	}
    	x1++;
    	ddF_x += 2;
    	f += ddF_x;

		Draw_Pixel(x + x1, y + y1);
    	Draw_Pixel(x - x1, y + y1);
		Draw_Pixel(x + x1, y - y1);
		Draw_Pixel(x - x1, y - y1);
		Draw_Pixel(x + y1, y + x1);
		Draw_Pixel(x - y1, y + x1);
		Draw_Pixel(x + y1, y - x1);
		Draw_Pixel(x - y1, y - x1);
 	}
}

//draw a circular bead
void Draw_Circle_Helper(int16_t x0, int16_t y0, int16_t radius, uint8_t cornername)
{
	int16_t f     = 1 - radius;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * radius;
	int16_t x     = 0;
	int16_t y     = radius;
	while (x<y)
	{
    	if (f >= 0)
		{
      		y--;
      		ddF_y += 2;
      		f += ddF_y;
    	}
	    x++;
	    ddF_x += 2;
	    f += ddF_x;
	    if (cornername & 0x4)
		{
			Draw_Pixel(x0 + x, y0 + y);
			Draw_Pixel(x0 + y, y0 + x);
	    }
	    if (cornername & 0x2)
		{
			Draw_Pixel(x0 + x, y0 - y);
			Draw_Pixel(x0 + y, y0 - x);
	    }
	    if (cornername & 0x8)
		{
			Draw_Pixel(x0 - y, y0 + x);
			Draw_Pixel(x0 - x, y0 + y);
	    }
	    if (cornername & 0x1)
		{
			Draw_Pixel(x0 - y, y0 - x);
	 		Draw_Pixel(x0 - x, y0 - y);
	    }
  	}
}

//fill a circle
void Fill_Circle(int16_t x, int16_t y, int16_t radius)
{
	Draw_Fast_VLine(x, y-radius, 2*radius+1);
	Fill_Circle_Helper(x, y, radius, 3, 0);
}

//fill a semi-circle
void Fill_Circle_Helper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername,int16_t delta)
{
	int16_t f     = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x     = 0;
	int16_t y     = r;

	while (x<y)
	{
    	if (f >= 0)
		{
      		y--;
      		ddF_y += 2;
      		f += ddF_y;
    	}
    	x++;
    	ddF_x += 2;
    	f += ddF_x;

    	if (cornername & 0x1)
		{
      		Draw_Fast_VLine(x0+x, y0-y, 2*y+1+delta);
      		Draw_Fast_VLine(x0+y, y0-x, 2*x+1+delta);
    	}
    	if (cornername & 0x2)
		{
      		Draw_Fast_VLine(x0-x, y0-y, 2*y+1+delta);
      		Draw_Fast_VLine(x0-y, y0-x, 2*x+1+delta);
    	}
  	}
}

//draw a vertical line
void Draw_Fast_VLine(int16_t x, int16_t y, int16_t h)
{
	Fill_Rect(x, y, 1, h, draw_color);
}

//draw a horizontal line
void Draw_Fast_HLine(int16_t x, int16_t y, int16_t w)
{
	Fill_Rect(x, y, w, 1, draw_color);
}


void Set_Text_Cousur(int16_t x, int16_t y)
{
	text_x = x;
	text_y = y;
}


//get text x coordinate
int16_t Get_Text_X_Cousur(void)
{
	return text_x;
}

//get text y coordinate
int16_t Get_Text_Y_Cousur(void)
{
	return text_y;
}

//set text colour with 16bit color
void Set_Text_colour(uint16_t color)
{
	text_color = color;
}


//get text colour
uint16_t Get_Text_colour(void)
{
	return text_color;
}

//set text background colour with 16bits color
void Set_Text_Back_colour(uint16_t color)
{
	text_bgcolor = color;
}

//get text background colour
uint16_t Get_Text_Back_colour(void)
{
	return text_bgcolor;
}

//set text size
void Set_Text_Size(uint8_t s)
{
	text_size = s;
}

//get text size
uint8_t Get_Text_Size(void)
{
	return text_size;
}

//set text mode
void Set_Text_Mode(bool mode)
{
	text_mode = mode;
}

//get text mode
bool Get_Text_Mode(void)
{
	return text_mode;
}

//draw a char
void Draw_Char(int16_t x, int16_t y, uint8_t c, uint16_t color,uint16_t bg, uint8_t size, bool mode)
{
	if((x >= Get_Width()) || (y >= Get_Height()) || ((x + 6 * size - 1) < 0) || ((y + 8 * size - 1) < 0))
	{
    	return;
	}
  	if(c >= 176)
  	{
		c++;
  	}
	for (int8_t i=0; i<6; i++)
	{
    	uint8_t line;
    	if (i == 5)
    	{
      		line = 0x0;
    	}
    	else
    	{
      		line = lcd_font[(c*5)+i];
    	}
    	for (int8_t j = 0; j<8; j++)
		{
      		if (line & 0x1)
			{
        		if (size == 1)
        		{
        			Draw_Pixe(x+i, y+j, color);
        		}
        		else
				{
					Fill_Rect(x+(i*size), y+(j*size), size, size, color);
        		}
        	}
			else if (bg != color)
			{
				if(!mode)
				{
	        		if (size == 1)
	        		{
	        			Draw_Pixe(x+i, y+j, bg);
	        		}
	        		else
					{
						Fill_Rect(x+i*size, y+j*size, size, size, bg);
					}
				}
			}
      		line >>= 1;
    	}
    }
}

//print string
size_t lcd_Print(uint8_t *st, int16_t x, int16_t y)
{
	int16_t pos;
	uint16_t len;
	const char * p = (const char *)st;
	size_t n = 0;
	if (x == CENTER || x == RIGHT)
	{
		len = strlen((const char *)st) * 6 * text_size;
		pos = (Get_Display_Width() - len);
		if (x == CENTER)
		{
			x = pos/2;
		}
		else
		{
			x = pos - 1;
		}
	}
    Set_Text_Cousur(x, y);
	while(1)
	{
		unsigned char ch = *(p++);//pgm_read_byte(p++);
		if(ch == 0)
		{
			break;
		}
		if(lcd_write(ch))
		{
			n++;
		}
		else
		{
			break;
		}
	}
	return n;
}

//print string
void lcd_Print_String(uint8_t *st, int16_t x, int16_t y)
{
	lcd_Print(st, x, y);
}

//print int number
void lcd_Print_Number_Int(long num, int16_t x, int16_t y, int16_t length, uint8_t filler, int16_t system)
{
	uint8_t st[27] = {0};
	uint8_t *p = st+26;
	bool flag = false;
	int16_t len = 0,nlen = 0,left_len = 0,i = 0;
	*p = '\0';
	if(0 == num)
	{
		*(--p) = '0';
		len = 1;
	}
	else
	{
		if(num < 0)
		{
			num = -num;
			flag = true;
		}
	}
	while((num > 0) && (len < 10))
	{
		if(num%system > 9)
		{
			*(--p) = 'A' + (num%system-10);
		}
		else
		{
			*(--p) = '0' + num%system;
		}
		num = num/system;
		len++;
	}
	if(flag)
	{
		*(--p) = '-';
	}
	if(length > (len + flag + 1))
	{
		if(length > sizeof(st))
		{
			nlen = sizeof(st) - len - flag - 1;
		}
		else
		{
			nlen = length - len - flag - 1;
		}
		for(i = 0;i< nlen;i++)
		{
			*(--p) = filler;
		}
		left_len = sizeof(st) - nlen - len - flag - 1;
	}
	else
	{
		left_len = sizeof(st) - len - flag - 1;
	}
	for(i = 0; i < (sizeof(st) - left_len);i++)
	{
		st[i] = st[left_len + i];
	}
	st[i] = '\0';
	lcd_Print(st, x, y);
}

//print float number
void Print_Number_Float(double num, uint8_t dec, int16_t x, int16_t y, uint8_t divider, int16_t length, uint8_t filler)
{
	uint8_t st[27] = {0};
	uint8_t * p = st;
	bool flag = false;
	int16_t i = 0;
	if(dec<1)
	{
		dec=1;
	}
	else if(dec>5)
	{
		dec=5;
	}
	if(num<0)
	{
		flag = true;
	}
	dtostrf(num, length, dec, (char *)st);
	if(divider != '.')
	{
		while(i < sizeof(st))
		{
			if('.' == *(p+i))
			{
				*(p+i) = divider;
			}
			i++;
		}
	}
	if(filler != ' ')
	{
		if(flag)
		{
			*p = '-';
			i = 1;
			while(i < sizeof(st))
			{
				if((*(p+i) == ' ') || (*(p+i) == '-'))
				{
					*(p+i) = filler;
				}
				i++;
			}
		}
		else
		{
			i = 0;
			while(i < sizeof(st))
			{
				if(' ' == *(p+i))
				{
					*(p+i) = filler;
				}
			}
		}
	}
	lcd_Print(st, x, y);
}

//write a char
size_t lcd_write(uint8_t c)
{
	if (c == '\n')
	{
    	text_y += text_size*8;
    	text_x  = 0;
 	}
	else if(c == '\r')
	{
	}
	else
	{
    	Draw_Char(text_x, text_y, c, text_color, text_bgcolor, text_size,text_mode);
    	text_x += text_size*6;
    }
  	return 1;
}

//get lcd width
int16_t Get_Display_Width(void)
{
	return Get_Width();
}

//get lcd height
int16_t Get_Display_Height(void)
{
	return Get_Height();
}
