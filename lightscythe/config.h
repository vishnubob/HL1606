#ifndef __CONFIG_H
#define __CONFIG_H

#include "HL1606strip.h"

/*
*   5V      - Or/Bl
*   Latch   - Wor
*   Clock   - Gr
*   Data    - Wgr
*   Sync    - Br
*   GND     - Wbr/Wbl
*
*   Debug Harness
*
*   5V      - 
*   Latch   - Yellow
*   Clock   - White
*   Data    - Gray
*   Sync    - Brown
*   GND     - Green
*/                   

// for the LED belt, these are the pins used
#define STRIP_LATCH     4
#define STRIP_CLOCK     5
#define STRIP_DATA      6
#define STRIP_SYNC      7
#define NUM_LEDS        70 
#define CS_PIN          8

//#define SERIAL_DEBUG

// for some odd reason, my HL1606 strip is coded
// this way.
#define _RED    0b100
#define _GRN    0b001
#define _BLU    0b010

// Pin maps
#define SPEED_DIAL_PIN  A1
#define AUTO_INC_PIN    A2
#define ONE_SHOT_PIN    A2
#define GO_PIN          A2
#define UP_PIN          A2
#define DOWN_PIN        A2

#define AUTO_INC_IDX    0
#define ONE_SHOT_IDX    1
#define GO_IDX          2
#define UP_IDX          3
#define DOWN_IDX        4

#define BUTTON_COUNT    5

/*
 * Interface
 * Speed dial
 * Auto-increment on / off
 * One-shot / loop
 * next / prev (buttons)
 * go (button)
 */

#endif // __CONFIG_H
