
#include"types.h"
void delay_usec(u32 delay_usec)
{

for(delay_usec*=12;delay_usec>0;delay_usec--);
}

void delay_msec(u32 delay_msec)
{

for(delay_msec*=12000;delay_msec>0;delay_msec--);
}

void delay_sec(u32 delay_sec)
{

for(delay_sec*=12000000;delay_sec>0;delay_sec--);
}
