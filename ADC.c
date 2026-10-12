#include <lpc21xx.h>
#include "types.h"
#include "ADC.h"
#include "delay.h"
#include "ADC_DEFINES"

void ADC_Init(void)
{
    // P0.28 = AD0.1
    // P0.29 = AD0.2 

    PINSEL1&=~(255<<((27-16)*2));
	//cfg p0.28 pinas AINO
	
    PINSEL1|=AIN1;
	PINSEL1|=AIN2;

   	PINSEL1|=0x05000000;//all pins adc  28to29

    ADCR=1<<PDN_BIT|(CLKDIV_VALUE<<CLKDIV);
}

void ADC_Read(u32 chno,u32* dval, f32* eAR)
{
	//clear previous channel values
	ADCR&=~(255<<0);
	//select channel & start conversion
	ADCR|=1<<chno|1<<start_conv;
	//wait for 3usec
	delay_usec(3);
	//check the done bit status
	while(((ADDR>>DONE_BIT)&1)==0);
	//stop conversion
	ADCR&=~(1<<start_conv);
	//extract 10bit digital output
	*dval= ((ADDR>>result)&1023);
	//find eAR value
	*eAR=(3.3/1023)*(*dval);
}


f32 LM35_ReadC(void)
{
    u32 dval;
	  f32 EAR;

    ADC_Read(channel,&dval,&EAR);

    return (EAR*100);
}

u32 MQ2_ReadGas(void)
{
    u32 dval;
    f32 EAR;

    ADC_Read(channel,&dval,&EAR);

    return dval;
}
