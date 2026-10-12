#include <lpc21xx.h>
#include "types.h"
#include "RTC.h"

#define FOSC 12000000
#define CCLK (5*FOSC)
#define PCLK (CCLK/4)
#define PREINT_VAL  ((u32)(PCLK/32768)-1)
#define PREFRAC_VAL (PCLK-((PREINT_VAL+1)*32768))

void RTC_Init(void)
{
    CCR = (1<<1);
    PREINT = PREINT_VAL;
    PREFRAC = PREFRAC_VAL;
    CCR = (1<<0);
}

void RTC_SetTime(u32 h,u32 m,u32 s)
{
    HOUR=h; MIN=m; SEC=s;
}

void RTC_GetTime(u32 *h,u32 *m,u32 *s)
{
    *h=HOUR; *m=MIN; *s=SEC;
}

void RTC_SetDate(u32 d,u32 mon,u32 y)
{
    DOM=d; MONTH=mon; YEAR=y;
}

void RTC_GetDate(u32 *d,u32 *mon,u32 *y)
{
    *d=DOM; *mon=MONTH; *y=YEAR;
}

u32 RTC_GetSecondsOfDay(void)
{
    return HOUR*3600 + MIN*60 + SEC;
}
