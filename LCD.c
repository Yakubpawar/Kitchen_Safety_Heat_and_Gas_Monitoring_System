#include <lpc21xx.h>
#include "types.h"
#include "delay.h"
#include "defines.h"
#include "LCD_defines.h"
#include "LCD.h"

void WRITE_LCD_CMD(u8 cmd)
{
    SCLRBIT(IOCLR0,LCD_RW);
	
    SCLRBIT(IOCLR0,LCD_RS);
	
    WRITEBYTE(IOPIN0,LCD_DATA,cmd);
	
    SSETBIT(IOSET0,LCD_EN);
	
    delay_usec(1);
	
    SCLRBIT(IOCLR0,LCD_EN);
	
    delay_msec(2);
}

void WRITE_LCD_DATA(u8 ascii)
{
    SCLRBIT(IOCLR0,LCD_RW);
	
    SSETBIT(IOSET0,LCD_RS);
	
    WRITEBYTE(IOPIN0,LCD_DATA,ascii);
	
    SSETBIT(IOSET0,LCD_EN);
	
    delay_usec(1);
	
    SCLRBIT(IOCLR0,LCD_EN);
	
    delay_msec(2);
}

void Init_LCD(void)
{
    WRITEBYTE(IODIR0,LCD_DATA,0xFF);
	
    SETBIT(IODIR0,LCD_RS);
	
    SETBIT(IODIR0,LCD_RW);
	
    SETBIT(IODIR0,LCD_EN);

    delay_msec(15);
	
    WRITE_LCD_CMD(0x38);
	
    delay_msec(5);
	
    WRITE_LCD_CMD(0x38);
	
    delay_usec(100);
	
    WRITE_LCD_CMD(0x38);
	
    WRITE_LCD_CMD(MODE_8BIT_2LINE);
		
    WRITE_LCD_CMD(DISP_ON_CUR_OFF);
		
    WRITE_LCD_CMD(CLEAR_LCD);
		
    WRITE_LCD_CMD(SHIFT_CUR_RIGHT);
}

void StrLCD(u8 *str)
{
    while(*str)
        WRITE_LCD_DATA(*str++);
}

void U32LCD(u32 n)
{
    u8 a[10];
    s32 i=0;
    if(n==0)
        WRITE_LCD_DATA('0');
    else
    {
        while(n)
        {
            a[i++] = n%10 + '0';
            n /= 10;
        }
        while(i--)
					
            WRITE_LCD_DATA(a[i]);
    }
}

void S32LCD(s32 n)
{
    if(n<0)
    {
        WRITE_LCD_DATA('-');
			
        n=-n;
    }
    U32LCD((u32)n);
}

void F32LCD(f32 fn, u8 nDP)
{
    u32 iNum,i;
    if(fn<0)
    {
        WRITE_LCD_DATA('-');
        fn=-fn;
    }
    iNum=(u32)fn;
		
    U32LCD(iNum);
		
    WRITE_LCD_DATA('.');
		
    for(i=0;i<nDP;i++)
    {
        fn=(fn-iNum)*10;
			
        iNum=(u32)fn;
			
        WRITE_LCD_DATA((u8)(iNum+'0'));
    }
}
