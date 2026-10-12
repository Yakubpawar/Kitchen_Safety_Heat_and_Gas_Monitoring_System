
#include "types.h"
void RTC_Init(void);
void RTC_SetTime(u32 h,u32 m,u32 s);
void RTC_GetTime(u32 *h,u32 *m,u32 *s);
void RTC_SetDate(u32 d,u32 mon,u32 y);
void RTC_GetDate(u32 *d,u32 *mon,u32 *y);
u32 RTC_GetSecondsOfDay(void);
