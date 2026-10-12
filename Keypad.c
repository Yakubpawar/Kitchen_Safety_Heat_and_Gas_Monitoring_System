#include <lpc21xx.h>
#include "types.h"
#include "delay.h"
#include "Keypad.h"

#define ROW_POS 16
#define COL_POS 20

static const char keymap[4][4]={
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}};

void Keypad_Init(void)
{
    // Rows P1.16-P1.19 output, columns P1.20-P1.23 input.
    IODIR1 |= (0x0F<<ROW_POS);
    IODIR1 &= ~(0x0F<<COL_POS);
    IOSET1 = (0x0F<<COL_POS);
}

char Keypad_GetKey(void)
{
    int r,c;
    u32 col;

    for(r=0;r<4;r++)
    {
        IOSET1 = (0x0F<<ROW_POS);
        IOCLR1 = (1<<(ROW_POS+r));
        delay_msec(1);

        col=(IOPIN1>>COL_POS)&0x0F;
        if(col!=0x0F)
        {
            for(c=0;c<4;c++)
            {
                if(!(col&(1<<c)))
                {
                    while((((IOPIN1>>COL_POS)&0x0F)) != 0x0F);
                    return keymap[r][c];
                }
            }
        }
    }
    return 0;
}
