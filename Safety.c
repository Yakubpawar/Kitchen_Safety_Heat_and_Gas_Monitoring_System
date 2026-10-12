#include <lpc21xx.h>
#include "types.h"
#include "delay.h"
#include "LCD.h"
#include "RTC.h"
#include "Safety.h"

extern u32 temp_threshold;
extern u32 gas_threshold;

static u32 alert_silenced = 0;

void Safety_Init(void)
{
    // LED as output
    IODIR0 |= (1 << TEMP_GAS_LED);

    // Buzzer as output
    IODIR1 |= (1 << BUZZER);

    // Switch as input
    IODIR0 &= ~(1 << SWITCH);

    // LED OFF
    IOCLR0 = (1 << TEMP_GAS_LED);

    // Buzzer OFF
    IOCLR1 = (1 << BUZZER);
}

void Safety_Update(u32 tempC, u32 gasRaw)
{
    // Check whether temperature and gas are normal
    if((tempC < temp_threshold) && (gasRaw < gas_threshold))
    {
        alert_silenced = 0;

        IOCLR0 = (1 << TEMP_GAS_LED);
        IOCLR1 = (1 << BUZZER);

        return;
    }

    // If alert was already silenced, keep LED and buzzer OFF
    if(alert_silenced == 1)
    {
        IOCLR0 = (1 << TEMP_GAS_LED);
        IOCLR1 = (1 << BUZZER);

        return;
    }

    // Temperature HIGH
    if(tempC >= temp_threshold)
    {
        IOSET0 = (1 << TEMP_GAS_LED);
        IOCLR1 = (1 << BUZZER);

        WRITE_LCD_CMD(0x01);
        WRITE_LCD_CMD(0x80);

        StrLCD((u8*)"TEMP HIGH:");
        U32LCD(tempC);

        WRITE_LCD_CMD(0xC0);
        StrLCD((u8*)"SAFETY ALERT");

        // Wait until switch is pressed
        while(((IOPIN0 >> SWITCH) & 1) == 1);

        // Switch pressed
        IOCLR1 = (1 << BUZZER);
        IOCLR0 = (1 << TEMP_GAS_LED);

        alert_silenced = 1;

        // Wait until switch is released
        while(((IOPIN0 >> SWITCH) & 1) == 0);
    }

    // Gas HIGH
    else if(gasRaw >= gas_threshold)
    {
        IOSET0 = (1 << TEMP_GAS_LED);
        IOSET1 = (1 << BUZZER);

        WRITE_LCD_CMD(0x01);
        WRITE_LCD_CMD(0x80);

        StrLCD((u8*)"GAS HIGH:");
        U32LCD(gasRaw);

        WRITE_LCD_CMD(0xC0);
        StrLCD((u8*)"SAFETY ALERT");

        // Wait until switch is pressed
        while(((IOPIN0 >> SWITCH) & 1) == 1);

        // Switch pressed
        IOCLR1 = (1 << BUZZER);
        IOCLR0 = (1 << TEMP_GAS_LED);

        alert_silenced = 1;

        // Wait until switch is released
        while(((IOPIN0 >> SWITCH) & 1) == 0);
    }
}
