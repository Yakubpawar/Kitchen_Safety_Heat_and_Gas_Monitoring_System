//ADC_DEFINES

#define ADC_CLK_HZ 3000000
#define FOSC 12000000
#define CCLK (5*FOSC)
#define PCLK (CCLK/4)
#define CLKDIV_VALUE ((PCLK/ADC_CLK_HZ)-1)

#define  DONE_BIT 31
#define  channel  1
//#define  channel1 2
#define  CLKDIV 8
#define  PDN_BIT 21
#define  result 6
#define  start_conv 24

#define AIN0  0x004000000
#define AIN1  0x01000000
#define AIN2  0x04000000
#define AIN3  0x100000000
