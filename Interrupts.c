//interrupts.c

#include <lpc21xx.h>
#include "Interrupts.h"
#include "types.h"

#define EINT0_CHNO  14
#define EINT0_BIT    1
#define IRQ_BIT      5

extern u32 edit_request;

void EINT0_ISR(void)__irq
{
    edit_request=1;

    EXTINT=(1<<EINT0_BIT);

    VICVectAddr=0;
}

void Interrupt_Init(void)
{
    // P0.1 EINT0

    PINSEL0&= ~(3<<2);
    PINSEL0|=(3<<2);

    // Falling-edge interrupt

    EXTMODE|=(1<<EINT0_BIT);

    EXTPOLAR&=~(1<<EINT0_BIT);

    // Select IRQ

    VICIntSelect&=~(1<<EINT0_CHNO);

    // Enable EINT0

    VICIntEnable|=(1<<EINT0_CHNO);

    // EINT0 vector

    VICVectCntl0=(1<<IRQ_BIT)|EINT0_CHNO;

    VICVectAddr0=(u32)EINT0_ISR;

    // Clear pending interrupt

    EXTINT=(1<<EINT0_BIT);
}

