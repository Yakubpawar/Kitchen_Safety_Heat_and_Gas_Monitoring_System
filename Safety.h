#include "types.h"
#define TEMP_GAS_LED 23
#define BUZZER       27
#define SWITCH       24

void Safety_Init(void);
void Safety_Update(u32 tempC, u32 gasRaw);
