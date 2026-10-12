// security.c

#include <lpc21xx.h>
#include "types.h"
#include "LCD.h"
#include "Keypad.h"
#include "RTC.h"
#include "Safety.h"
#include "delay.h"
#include "Security.h"


/*================================================
              EXTERNAL VARIABLES
================================================*/

extern u32 temp_threshold;
extern u32 gas_threshold;


/*================================================
              PASSWORD
================================================*/

char password[5] = {'1','2','3','4','\0'};


/*================================================
              DISPLAY ONE LINE
================================================*/

void show_line(const char *s)
{
    WRITE_LCD_CMD(0x01);
    StrLCD((u8*)s);
}


/*================================================
              GET KEY - BLOCKING
================================================*/

char get_key_blocking(void)
{
    char k = 0;

    while(!k)
        k = Keypad_GetKey();

    return k;
}


/*================================================
              READ 4 DIGIT PASSWORD
================================================*/

int read_four_digits(char *buf)
{
    int i = 0;
    char k;

    while(i < 4)
    {
        k = get_key_blocking();

        /* DIGIT */

        if(k >= '0' && k <= '9')
        {
            buf[i] = k;
            i++;

            WRITE_LCD_DATA('*');
        }

        /* BACKSPACE */

        else if(k == 'D')
        {
            if(i > 0)
            {
                i--;

                WRITE_LCD_CMD(0x10);
                WRITE_LCD_DATA(' ');
                WRITE_LCD_CMD(0x10);
            }
        }

        /* CANCEL */

        else if(k == '*')
        {
            return 0;
        }
    }

    buf[4] = '\0';

    return 1;
}


/*================================================
              CHECK PASSWORD
================================================*/

int check_password(void)
{
    char entered[5];
    int i;

    show_line("Password:");

    if(!read_four_digits(entered))
        return 0;

    for(i = 0; i < 4; i++)
    {
        if(entered[i] != password[i])
            return 0;
    }

    return 1;
}


/*================================================
              READ POSITIVE NUMBER
================================================*/

u32 read_number(const char *title, u32 old, u32 max)
{
    int i = 0;
    char k;
    u32 n = 0;

    show_line(title);

    WRITE_LCD_CMD(0xC0);

    while(i < 6)
    {
        k = get_key_blocking();

        /* NUMBER */

        if(k >= '0' && k <= '9')
        {
            if(n <= (max - (k - '0')) / 10)
            {
                n = n * 10 + (k - '0');

                WRITE_LCD_DATA(k);

                i++;
            }
        }

        /* BACKSPACE */

        else if(k == 'D')
        {
            if(i > 0)
            {
                i--;

                n = n / 10;

                WRITE_LCD_CMD(0x10);
                WRITE_LCD_DATA(' ');
                WRITE_LCD_CMD(0x10);
            }
        }

        /* SAVE */

        else if(k == '#')
        {
            return n;
        }

        /* CANCEL */

        else if(k == '*')
        {
            return old;
        }
    }

    return n;
}


/*================================================
              READ TEMPERATURE
              RANGE: -50 TO +150
              
              A = -
              # = SAVE
              * = CANCEL
              D = CLEAR
================================================*/

s32 read_temperature(const char *title, s32 old)
{
    char k;

    s32 value = 0;

    int sign = 1;
    int digits = 0;


    show_line(title);

    WRITE_LCD_CMD(0xC0);


    while(1)
    {
        k = get_key_blocking();


        /*----------------------------------------
                    NEGATIVE SIGN
                    A = -
        ----------------------------------------*/

        if(k == 'A' && digits == 0)
        {
            sign = -1;

            WRITE_LCD_CMD(0xC0);

            WRITE_LCD_DATA('-');
        }


        /*----------------------------------------
                    DIGIT
        ----------------------------------------*/

        else if(k >= '0' && k <= '9')
        {
            if(digits < 3)
            {
                value = (value * 10) + (k - '0');

                digits++;


                WRITE_LCD_CMD(0xC0);


                if(sign == -1)
                    WRITE_LCD_DATA('-');


                if(value >= 100)
                {
                    WRITE_LCD_DATA((value / 100) + '0');

                    WRITE_LCD_DATA(((value / 10) % 10) + '0');

                    WRITE_LCD_DATA((value % 10) + '0');
                }
                else if(value >= 10)
                {
                    WRITE_LCD_DATA((value / 10) + '0');

                    WRITE_LCD_DATA((value % 10) + '0');
                }
                else
                {
                    WRITE_LCD_DATA(value + '0');
                }
            }
        }


        /*----------------------------------------
                    SAVE
                    # = SAVE
        ----------------------------------------*/

        else if(k == '#')
        {
            if(digits == 0)
                return old;

            value = value * sign;

            return value;
        }


        /*----------------------------------------
                    CANCEL
                    * = CANCEL
        ----------------------------------------*/

        else if(k == '*')
        {
            return old;
        }


        /*----------------------------------------
                    CLEAR
                    D = CLEAR
        ----------------------------------------*/

        else if(k == 'D')
        {
            value = 0;

            digits = 0;

            sign = 1;

            WRITE_LCD_CMD(0xC0);

            StrLCD((u8*)"                ");

            WRITE_LCD_CMD(0xC0);
        }
    }
}


/*================================================
              EDIT TEMPERATURE / GAS
================================================*/

void edit_thresholds(void)
{
    char k;

    s32 new_temp;


    while(1)
    {
        /*----------------------------------------
                  THRESHOLD MENU
        ----------------------------------------*/

        WRITE_LCD_CMD(0x01);

        StrLCD((u8*)"A:TEMP B:GAS");

        WRITE_LCD_CMD(0xC0);

        StrLCD((u8*)"C:EXIT");


        k = get_key_blocking();


        /*========================================
                    TEMPERATURE
        ========================================*/

        if(k == 'A')
        {
            new_temp =
                read_temperature("Temp -50 to 150",
                                 temp_threshold);


            if(new_temp < -50 || new_temp > 150)
            {
                show_line("Invalid Temp");

                delay_msec(1000);
            }
            else
            {
                temp_threshold = new_temp;

                show_line("Temp Saved");

                delay_msec(800);
            }
        }


        /*========================================
                    GAS
        ========================================*/

        else if(k == 'B')
        {
            gas_threshold =
                read_number("Gas 0-1023",
                            gas_threshold,
                            1023);

            show_line("Gas Saved");

            delay_msec(800);
        }


        /*========================================
                    EXIT
        ========================================*/

        else if(k == 'C')
        {
            break;
        }
    }
}


/*================================================
              EDIT TIME
================================================*/

void edit_time_values(void)
{
    u32 h;
    u32 m;
    u32 s;

    u32 new_value;

    char k;


    while(1)
    {
        WRITE_LCD_CMD(0x01);

        StrLCD((u8*)"A:HOUR B:MIN");

        WRITE_LCD_CMD(0xC0);

        StrLCD((u8*)"C:SEC D:EXIT");


        k = get_key_blocking();


        /*========================================
                    HOUR
                    VALID: 0 - 23
        ========================================*/

        if(k == 'A')
        {
            RTC_GetTime(&h, &m, &s);


            new_value =
                read_number("Hour 0-23",
                            h,
                            999);


            if(new_value > 23)
            {
                show_line("Invalid Hour");

                delay_msec(1000);
            }
            else
            {
                h = new_value;

                RTC_SetTime(h, m, s);

                show_line("Hour Saved");

                delay_msec(800);
            }
        }


        /*========================================
                    MINUTE
                    VALID: 0 - 59
        ========================================*/

        else if(k == 'B')
        {
            RTC_GetTime(&h, &m, &s);


            new_value =
                read_number("Minute 0-59",
                            m,
                            999);


            if(new_value > 59)
            {
                show_line("Invalid Minute");

                delay_msec(1000);
            }
            else
            {
                m = new_value;

                RTC_SetTime(h, m, s);

                show_line("Minute Saved");

                delay_msec(800);
            }
        }


        /*========================================
                    SECOND
                    VALID: 0 - 59
        ========================================*/

        else if(k == 'C')
        {
            RTC_GetTime(&h, &m, &s);


            new_value =
                read_number("Second 0-59",
                            s,
                            999);


            if(new_value > 59)
            {
                show_line("Invalid Second");

                delay_msec(1000);
            }
            else
            {
                s = new_value;

                RTC_SetTime(h, m, s);

                show_line("Second Saved");

                delay_msec(800);
            }
        }


        /*========================================
                    EXIT
        ========================================*/

        else if(k == 'D')
        {
            break;
        }
    }
}


/*================================================
              EDIT DATE
================================================*/

void edit_date_values(void)
{
    u32 d;
    u32 mo;
    u32 y;

    u32 new_value;

    char k;


    while(1)
    {
        WRITE_LCD_CMD(0x01);

        StrLCD((u8*)"A:DATE B:MONTH");

        WRITE_LCD_CMD(0xC0);

        StrLCD((u8*)"C:YEAR D:EXIT");


        k = get_key_blocking();


        /*========================================
                    DATE
                    VALID: 1 - 31
        ========================================*/

        if(k == 'A')
        {
            RTC_GetDate(&d, &mo, &y);


            new_value =
                read_number("Date 1-31",
                            d,
                            999);


            if(new_value < 1 || new_value > 31)
            {
                show_line("Invalid Date");

                delay_msec(1000);
            }
            else
            {
                d = new_value;

                RTC_SetDate(d, mo, y);

                show_line("Date Saved");

                delay_msec(800);
            }
        }


        /*========================================
                    MONTH
                    VALID: 1 - 12
        ========================================*/

        else if(k == 'B')
        {
            RTC_GetDate(&d, &mo, &y);


            new_value =
                read_number("Month 1-12",
                            mo,
                            999);


            if(new_value < 1 || new_value > 12)
            {
                show_line("Invalid Month");

                delay_msec(1000);
            }
            else
            {
                mo = new_value;

                RTC_SetDate(d, mo, y);

                show_line("Month Saved");

                delay_msec(800);
            }
        }


        /*========================================
                    YEAR
                    VALID: 0 - 9999
        ========================================*/

        else if(k == 'C')
        {
            RTC_GetDate(&d, &mo, &y);


            new_value =
                read_number("Year 0-9999",
                            y,
                            9999);


            if(new_value > 9999)
            {
                show_line("Invalid Year");

                delay_msec(1000);
            }
            else
            {
                y = new_value;

                RTC_SetDate(d, mo, y);

                show_line("Year Saved");

                delay_msec(800);
            }
        }


        /*========================================
                    EXIT
        ========================================*/

        else if(k == 'D')
        {
            break;
        }
    }
}


/*================================================
              RTC MAIN MENU
================================================*/

void edit_rtc(void)
{
    char k;


    while(1)
    {
        WRITE_LCD_CMD(0x01);

        StrLCD((u8*)"A:TIME B:DATE");

        WRITE_LCD_CMD(0xC0);

        StrLCD((u8*)"C:EXIT");


        k = get_key_blocking();


        /*========================================
                    TIME
        ========================================*/

        if(k == 'A')
        {
            edit_time_values();
        }


        /*========================================
                    DATE
        ========================================*/

        else if(k == 'B')
        {
            edit_date_values();
        }


        /*========================================
                    EXIT
        ========================================*/

        else if(k == 'C')
        {
            break;
        }
    }
}


/*================================================
              CHANGE PASSWORD
================================================*/

void change_password(void)
{
    char newp[5];

    int i;


    show_line("New Password:");


    if(read_four_digits(newp))
    {
        for(i = 0; i < 4; i++)
            password[i] = newp[i];


        show_line("Password Saved");

        delay_msec(1200);
    }
}


/*================================================
              WRONG PASSWORD ALERT
================================================*/

void Wrong_Password_Alert(void)
{
    /* LED ON */

    IOSET0 = (1 << TEMP_GAS_LED);


    /* BUZZER ON */

    IOSET0 = (1 << BUZZER);


    delay_msec(1000);


    /* LED OFF */

    IOCLR0 = (1 << TEMP_GAS_LED);


    /* BUZZER OFF */

    IOCLR0 = (1 << BUZZER);
}


/*================================================
              SECURITY EDIT MODE
================================================*/

void Security_EditMode(void)
{
    int i;

    int sec;

    char k;


    while(1)
    {
        /*========================================
              GIVE 3 PASSWORD ATTEMPTS
        ========================================*/

        i = 3;


        while(i > 0)
        {
            /*------------------------------------
                    ASK PASSWORD
            ------------------------------------*/

            if(check_password())
            {
                /* Correct password */

                IOCLR0 = (1 << TEMP_GAS_LED);

                IOCLR0 = (1 << BUZZER);

                break;
            }


            /*------------------------------------
                    WRONG PASSWORD
            ------------------------------------*/

            i--;


            show_line("Wrong Password");


            /* LED ON */

            IOSET0 = (1 << TEMP_GAS_LED);


            /* BUZZER ON */

            IOSET0 = (1 << BUZZER);


            delay_msec(1000);


            /* LED OFF */

            IOCLR0 = (1 << TEMP_GAS_LED);


            /* BUZZER OFF */

            IOCLR0 = (1 << BUZZER);


            delay_msec(500);
        }


        /*========================================
              THREE WRONG PASSWORDS
        ========================================*/

        if(i == 0)
        {
            for(sec = 30; sec >= 0; sec--)
            {
                WRITE_LCD_CMD(0x01);

                StrLCD((u8*)"SYSTEM LOCKED");


                WRITE_LCD_CMD(0xC0);

                StrLCD((u8*)"WAIT: ");


                U32LCD(sec);


                StrLCD((u8*)" SEC");


                if(sec > 0)
                    delay_sec(1000);
            }


            /* Start password entry again */

            continue;
        }


        /*========================================
              CORRECT PASSWORD - EDIT MENU
        ========================================*/

        while(1)
        {
            WRITE_LCD_CMD(0x01);

            StrLCD((u8*)"A:TH B:RTC");


            WRITE_LCD_CMD(0xC0);

            StrLCD((u8*)"C:PASS D:EXIT");


            k = get_key_blocking();


            /*------------------------------------
                    TEMPERATURE / GAS
            ------------------------------------*/

            if(k == 'A')
            {
                edit_thresholds();
            }


            /*------------------------------------
                    RTC
            ------------------------------------*/

            else if(k == 'B')
            {
                edit_rtc();
            }


            /*------------------------------------
                    PASSWORD
            ------------------------------------*/

            else if(k == 'C')
            {
                change_password();
            }


            /*------------------------------------
                    EXIT
            ------------------------------------*/

            else if(k == 'D')
            {
                break;
            }
        }


        /*========================================
                    EDIT SAVED
        ========================================*/

        show_line("Edit Saved");

        delay_msec(1000);


        break;
    }
}
