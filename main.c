#include <lpc21xx.h>
#include "types.h"
#include "delay.h"
#include "LCD.h"
#include "RTC.h"
#include "ADC.h"
#include "Keypad.h"
#include "Safety.h"
#include "Interrupts.h"
#include "Security.h"
#include "ADC_DEFINES.h"

u32 temp_threshold=30;
u32 gas_threshold=90;
u32 edit_request=0;

void display_normal(u32 tempC,u32 gas,u32 h,u32 m,u32 s,u32 d,u32 mN,u32 y)
{
    WRITE_LCD_CMD(0x01);
    WRITE_LCD_CMD(0x80);

    if(h<10)
        WRITE_LCD_DATA('0');
    U32LCD(h);

    WRITE_LCD_DATA(':');

    if(m<10)
        WRITE_LCD_DATA('0');
    U32LCD(m);

    WRITE_LCD_DATA(':');

    if(s<10)
        WRITE_LCD_DATA('0');
    U32LCD(s);

    WRITE_LCD_DATA(' ');

    StrLCD((u8*)"T:");
    U32LCD(tempC);
    WRITE_LCD_DATA(0xdf);
    StrLCD((u8*)"C");

    WRITE_LCD_CMD(0xC0);

    if(d<10)
        WRITE_LCD_DATA('0');
    U32LCD(d);

    WRITE_LCD_DATA('/');

    if(mN<10)
        WRITE_LCD_DATA('0');
    U32LCD(mN);

    WRITE_LCD_DATA('/');

    U32LCD(y);

    WRITE_LCD_DATA(' ');

    StrLCD((u8*)"G:");
    U32LCD(gas);

    delay_msec(1500);
}

int main(void)
{
	
    u32 tempC,gas,h,m,s,d,mN,y;
    
	IODIR0|=1<<7;
    Init_LCD();
    RTC_Init();
    ADC_Init();
    Keypad_Init();
    Safety_Init();
    Interrupt_Init();
	
    RTC_GetTime(&h,&m,&s);
    RTC_GetDate(&d,&mN,&y);
	
    WRITE_LCD_CMD(0x01);
    StrLCD((u8*)"KITCHEN SAFETY");
    WRITE_LCD_CMD(0xC0);
    StrLCD((u8*)"SYSTEM STARTING");
    delay_msec(1000);

    WRITE_LCD_CMD(0x01);
    StrLCD((u8*)"STAY SAFE!");
    WRITE_LCD_CMD(0xC0);
    StrLCD((u8*)"MONITOR ACTIVE");
    delay_msec(1000);

    while(1)
    {
			IOSET0=(1<<7);
        // Edit mode 
        if(edit_request)
        {
            edit_request=0;

            WRITE_LCD_CMD(0x01);
            StrLCD((u8*)"EINT0 WORKING");
            delay_msec(2000);

            Security_EditMode();
        }

        //Read sensors 
        tempC=LM35_ReadC();
        gas=MQ2_ReadGas();

        //Read RTC 
        RTC_GetTime(&h,&m,&s);
        RTC_GetDate(&d,&mN,&y);

        //Check safety 
        Safety_Update(tempC,gas);

        // Display normal condition 
        if(tempC<temp_threshold && gas<gas_threshold)
        {
            display_normal(tempC,gas,h,m,s,d,mN,y);
        }

        delay_sec(1);
				IOCLR0=1<<7;
    }
}
