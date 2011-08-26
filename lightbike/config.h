#ifndef __CONFIG_H
#define __CONFIG_H

#include <WProgram.h>
#include "HL1606stripPWM.h"

#define RED 1
#define GRN 2
#define BLU 4

typedef struct _rgb
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
} _rgb_t;

#define LONG_COUNT      70
#define SHORT_COUNT     48
#define LED_MODE_PIN    A5
#define TOTAL_MODES     4

#define SERIAL_DEBUG

#endif // __CONFIG_H
