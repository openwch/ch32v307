/********************************** (C) COPYRIGHT *******************************
* File Name          : lcd.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2024/03/05
* Description        : Main Interrupt Service Routines.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#include "lcd.h"
#include "lcd_init.h"
#include "lcdfont.h"
#include "debug.h"

/*********************************************************************
 * @fn      LCD_Fill
 *
 * @brief   Fill color in designated area
 *
 * @param   xsta,ysta - Starting coordinates
 *          xend,yend - Termination coordinates
 *          color     - The color to be filled
 *
 * @return  none
 */
void LCD_Fill(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color)
{          
	u16 i,j; 
	LCD_Address_Set(xsta,ysta,xend-1,yend-1);//Set display range
	for(i=ysta;i<yend;i++)
	{													   	 	
		for(j=xsta;j<xend;j++)
		{
			LCD_WR_DATA(color);
		}
	} 					  	    
}

/*********************************************************************
 * @fn      LCD_DrawPoint
 *
 * @brief   Draw a point at the designated location
 *
 * @param   x,y    - Draw the coordinates of the points
 *          color  - Points Color
 *
 * @return  none
 */

void LCD_DrawPoint(u16 x,u16 y,u16 color)
{
	LCD_Address_Set(x,y,x,y);//Set cursor position
	LCD_WR_DATA(color);
} 

/*********************************************************************
 * @fn      LCD_DrawLine
 *
 * @brief   Draw a line
 *
 * @param   x1,y1 - Starting coordinates
 *          x2,y2 - Termination coordinates
 *          color - Line color
 *
 * @return  none
 */
void LCD_DrawLine(u16 x1,u16 y1,u16 x2,u16 y2,u16 color)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance;
	int incx,incy,uRow,uCol;
	delta_x=x2-x1; //Calculate coordinate increment
	delta_y=y2-y1;
	uRow=x1;//Starting point coordinates of the line drawing
	uCol=y1;
	if(delta_x>0)incx=1; //Set single step direction
	else if (delta_x==0)incx=0;//vertical line
	else {incx=-1;delta_x=-delta_x;}
	if(delta_y>0)incy=1;
	else if (delta_y==0)incy=0;//horizontal line
	else {incy=-1;delta_y=-delta_x;}
	if(delta_x>delta_y)distance=delta_x; //Select the basic incremental coordinate axis
	else distance=delta_y;
	for(t=0;t<distance+1;t++)
	{
		LCD_DrawPoint(uRow,uCol,color);//point
		xerr+=delta_x;
		yerr+=delta_y;
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

/*********************************************************************
 * @fn      LCD_DrawRectangle
 *
 * @brief   Draw a rectangle
 *
 * @param   x1,y1 - Starting coordinates
 *          x2,y2 - Termination coordinates
 *          color - The color of the rectangle
 *
 * @return  none
 */

void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2,u16 color)
{
	LCD_DrawLine(x1,y1,x2,y1,color);
	LCD_DrawLine(x1,y1,x1,y2,color);
	LCD_DrawLine(x1,y2,x2,y2,color);
	LCD_DrawLine(x2,y1,x2,y2,color);
}

/*********************************************************************
 * @fn      Draw_Circle
 *
 * @brief   Draw a circle
 *
 * @param   x0,y0 - Center coordinates
 *          r     - radius
 *          color - The color of the circle
 *
 * @return  none
 */

void Draw_Circle(u16 x0,u16 y0,u8 r,u16 color)
{
	int a,b;
	a=0;b=r;	  
	while(a<=b)
	{
		LCD_DrawPoint(x0-b,y0-a,color);             //3           
		LCD_DrawPoint(x0+b,y0-a,color);             //0           
		LCD_DrawPoint(x0-a,y0+b,color);             //1                
		LCD_DrawPoint(x0-a,y0-b,color);             //2             
		LCD_DrawPoint(x0+b,y0+a,color);             //4               
		LCD_DrawPoint(x0+a,y0-b,color);             //5
		LCD_DrawPoint(x0+a,y0+b,color);             //6 
		LCD_DrawPoint(x0-b,y0+a,color);             //7
		a++;
		if((a*a+b*b)>(r*r))//Determine if the point to be drawn is too far away
		{
			b--;
		}
	}
}
/*********************************************************************
 * @fn      LCD_ShowChinese
 *
 * @brief   Display Chinese character strings
 *
 * @param   x,y   - display coordinates
 *          *s    - Chinese character string to be displayed
 *          fc    - Color of Words
 *          bc    - Background color of characters
 *          sizey - word size:16/24/32
 *          mode  - 0£ºNon stacking mode 1£º stacking mode
 *
 * @return  none
 */

void LCD_ShowChinese(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode)
{
	while(*s!=0)
	{
		if(sizey==16) LCD_ShowChinese16x16(x,y,s,fc,bc,sizey,mode);
		else if(sizey==24) LCD_ShowChinese24x24(x,y,s,fc,bc,sizey,mode);
		else if(sizey==32) LCD_ShowChinese32x32(x,y,s,fc,bc,sizey,mode);
		else return;
		s+=2;
		x+=sizey;
	}
}

/*********************************************************************
 * @fn      LCD_ShowChinese16x16
 *
 * @brief   Display a single 16x16 Chinese character
 *
 * @param   x,y   - display coordinates
 *          *s    - Chinese character string to be displayed
 *          fc    - Color of Words
 *          bc    - Background color of characters
 *          sizey - word size:16/24/32
 *          mode  - 0£ºNon stacking mode 1£º stacking mode
 *
 * @return  none
 */

void LCD_ShowChinese16x16(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode)
{
	u8 i,j;
	u16 k;
	u16 HZnum;
	u16 TypefaceNum;
	u16 x0=x;
	TypefaceNum=sizey/8*sizey;

	HZnum=sizeof(tfont16)/sizeof(typFNT_GB16);
	for(k=0;k<HZnum;k++) 
	{
		if ((tfont16[k].Index[0]==*(s))&&(tfont16[k].Index[1]==*(s+1)))
		{ 	
			LCD_Address_Set(x,y,x+sizey-1,y+sizey-1);
			for(i=0;i<TypefaceNum;i++)
			{
				for(j=0;j<8;j++)
				{	
					if(!mode)
					{
						if(tfont16[k].Msk[i]&(0x01<<j))LCD_WR_DATA(fc);
						else LCD_WR_DATA(bc);
					}
					else
					{
						if(tfont16[k].Msk[i]&(0x01<<j))	LCD_DrawPoint(x,y,fc);
						x++;
						if((x-x0)==sizey)
						{
							x=x0;
							y++;
							break;
						}
					}
				}
			}
		}				  	
		continue;
	}
} 

/*********************************************************************
 * @fn      LCD_ShowChinese24x24
 *
 * @brief   Display a single 24x24 Chinese character
 *
 * @param   x,y   - display coordinates
 *          *s    - Chinese character string to be displayed
 *          fc    - Color of Words
 *          bc    - Background color of characters
 *          sizey - word size:16/24/32
 *          mode  - 0£ºNon stacking mode 1£º stacking mode
 *
 * @return  none
 */

void LCD_ShowChinese24x24(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode)
{
	u8 i,j;
	u16 k;
	u16 HZnum;
	u16 TypefaceNum;
	u16 x0=x;
	TypefaceNum=sizey/8*sizey;
	HZnum=sizeof(tfont24)/sizeof(typFNT_GB24);
	for(k=0;k<HZnum;k++) 
	{
		if ((tfont24[k].Index[0]==*(s))&&(tfont24[k].Index[1]==*(s+1)))
		{ 	
			LCD_Address_Set(x,y,x+sizey-1,y+sizey-1);
			for(i=0;i<TypefaceNum;i++)
			{
				for(j=0;j<8;j++)
				{	
					if(!mode)
					{
						if(tfont24[k].Msk[i]&(0x01<<j))LCD_WR_DATA(fc);
						else LCD_WR_DATA(bc);
					}
					else
					{
						if(tfont24[k].Msk[i]&(0x01<<j))	LCD_DrawPoint(x,y,fc);
						x++;
						if((x-x0)==sizey)
						{
							x=x0;
							y++;
							break;
						}
					}
				}
			}
		}				  	
		continue;  //If the corresponding dot matrix font library is found, exit immediately to prevent the impact of multiple Chinese characters being duplicated
	}
} 

/*********************************************************************
 * @fn      LCD_ShowChinese32x32
 *
 * @brief   Display a single 32x32 Chinese character
 *
 * @param   x,y   - display coordinates
 *          *s    - Chinese character string to be displayed
 *          fc    - Color of Words
 *          bc    - Background color of characters
 *          sizey - word size:16/24/32
 *          mode  - 0£ºNon stacking mode 1£º stacking mode
 *
 * @return  none
 */

void LCD_ShowChinese32x32(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode)
{
	u8 i,j;
	u16 k;
	u16 HZnum;//Number of Chinese characters
	u16 TypefaceNum;//The byte size occupied by one character
	u16 x0=x;
	TypefaceNum=sizey/8*sizey;//This algorithm is only applicable to words whose width is equal to their height and whose height is a multiple of 8,
	                          //It is also recommended that users use words of this size, otherwise there may be display issues!
	HZnum=sizeof(tfont32)/sizeof(typFNT_GB32);	//Count the number of Chinese characters
	for(k=0;k<HZnum;k++) 
	{
		if ((tfont32[k].Index[0]==*(s))&&(tfont32[k].Index[1]==*(s+1)))
		{ 	
			LCD_Address_Set(x,y,x+sizey-1,y+sizey-1);
			for(i=0;i<TypefaceNum;i++)
			{
				for(j=0;j<8;j++)
				{	
					if(!mode)//Non stacking method
					{
						if(tfont32[k].Msk[i]&(0x01<<j))LCD_WR_DATA(fc);
						else LCD_WR_DATA(bc);
					}
					else//Stacking method
					{
						if(tfont32[k].Msk[i]&(0x01<<j))	LCD_DrawPoint(x,y,fc);//Draw a point
						x++;
						if((x-x0)==sizey)
						{
							x=x0;
							y++;
							break;
						}
					}
				}
			}
		}				  	
		continue;  //If the corresponding dot matrix font library is found, exit immediately to prevent the impact of multiple Chinese characters being duplicated
	}
}

/*********************************************************************
 * @fn      LCD_ShowChar
 *
 * @brief   Display a single character
 *
 * @param   x,y   - display coordinates
 *          num   - Chinese character string to be displayed
 *          fc    - Color of Words
 *          bc    - Background color of characters
 *          sizey - word size:16/24/32
 *          mode  - 0£ºNon stacking mode 1£º stacking mode
 *
 * @return  none
 */

void LCD_ShowChar(u16 x,u16 y,u8 num,u16 fc,u16 bc,u8 sizey,u8 mode)
{
	u8 temp,sizex,t;
	u16 i,TypefaceNum;//The byte size occupied by one character
	u16 x0=x;
	sizex=sizey/2;
	TypefaceNum=sizex/8*sizey;
	num=num-' ';    //Obtain the offset value
	LCD_Address_Set(x,y,x+sizex-1,y+sizey-1);  //Set cursor position
	for(i=0;i<TypefaceNum;i++)
	{ 
		if(sizey==16)temp=ascii_1608[num][i];		       //Call 8x16 font
		else if(sizey==32)temp=ascii_3216[num][i];		 //Call 16x32 font
		else return;
		for(t=0;t<8;t++)
		{
			if(!mode)//Non stacking mode
			{
				if(temp&(0x01<<t))LCD_WR_DATA(fc);
				else LCD_WR_DATA(bc);
			}
			else//Overlay mode
			{
				if(temp&(0x01<<t))LCD_DrawPoint(x,y,fc);//Draw a point
				x++;
				if((x-x0)==sizex)
				{
					x=x0;
					y++;
					break;
				}
			}
		}
	}   	 	  
}

/*********************************************************************
 * @fn      LCD_ShowString
 *
 * @brief   display string
 *
 * @param   x,y   - display coordinates
 *          *p    - Chinese character string to be displayed
 *          fc    - Color of Words
 *          bc    - Background color of characters
 *          sizey - word size:16/24/32
 *          mode  - 0£ºNon stacking mode 1£º stacking mode
 *
 * @return  none
 */


void LCD_ShowString(u16 x,u16 y,const u8 *p,u16 fc,u16 bc,u8 sizey,u8 mode)
{         
	while(*p!='\0')
	{       
		LCD_ShowChar(x,y,*p,fc,bc,sizey,mode);
		x+=sizey/2;
		p++;
	}  
}
/*********************************************************************
 * @fn      mypow
 *
 * @brief   Display numbers
 *
 * @param   m - base number
 *          n - index
 *
 * @return  none
 */

u32 mypow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;
	return result;
}

/*********************************************************************
 * @fn      LCD_ShowIntNum
 *
 * @brief   Display integer variables
 *
 * @param   x,y   - display coordinates
 *          num   - To display integer variables
 *          len   - Number of digits to display
 *          fc    - Color of Words
 *          bc    - Background color of characters
 *          sizey - word size:16/24/32
 *
 * @return  none
 */
void LCD_ShowIntNum(u16 x,u16 y,u16 num,u8 len,u16 fc,u16 bc,u8 sizey)
{         	
	u8 t,temp;
	u8 enshow=0;
	u8 sizex=sizey/2;
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+t*sizex,y,' ',fc,bc,sizey,0);
				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+t*sizex,y,temp+48,fc,bc,sizey,0);
	}
} 

/*********************************************************************
 * @fn      LCD_ShowFloatNum1
 *
 * @brief   Display integer variables
 *
 * @param   x,y   - Display two decimal variables
 *          num   - To display decimal variables
 *          len   - Number of digits to display
 *          fc    - Color of Words
 *          bc    - Background color of characters
 *          sizey - word size:16/24/32
 *
 * @return  none
 */
void LCD_ShowFloatNum1(u16 x,u16 y,float num,u8 len,u16 fc,u16 bc,u8 sizey)
{         	
	u8 t,temp,sizex;
	u16 num1;
	sizex=sizey/2;
	num1=num*100;
	for(t=0;t<len;t++)
	{
		temp=(num1/mypow(10,len-t-1))%10;
		if(t==(len-2))
		{
			LCD_ShowChar(x+(len-2)*sizex,y,'.',fc,bc,sizey,0);
			t++;
			len+=1;
		}
	 	LCD_ShowChar(x+t*sizex,y,temp+48,fc,bc,sizey,0);
	}
}

/*********************************************************************
 * @fn      LCD_ShowPicture
 *
 * @brief   display picture
 *
 * @param   x,y    - Display two decimal variables
 *          length - Image length
 *          width  - picture width
 *          pic[]  - Image array
 * @return  none
 */
void LCD_ShowPicture(u16 x,u16 y,u16 length,u16 width,const u8 pic[])
{
	u16 i,j,k=0;
	LCD_Address_Set(x,y,x+length-1,y+width-1);
	for(i=0;i<length;i++)
	{
		for(j=0;j<width;j++)
		{
			LCD_WR_DATA8(pic[k*2]);
			LCD_WR_DATA8(pic[k*2+1]);
			k++;
		}
	}			
}


