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
