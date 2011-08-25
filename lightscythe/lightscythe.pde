#include "config.h"
#include <SD.h>
 
/*******************************************************************************
 ** Globals
 ******************************************************************************/

HL1606strip     strip = HL1606strip(STRIP_DATA, STRIP_LATCH, STRIP_CLOCK, NUM_LEDS);
uint8_t         led_buffer[NUM_LEDS];

uint16_t        image_count;
uint16_t        current_image;

/* buttons */
bool            button_state[BUTTON_COUNT]; 
const uint8_t   _pin_map[] = {AUTO_INC_PIN, ONE_SHOT_PIN, GO_PIN, UP_PIN, DOWN_PIN};

/*******************************************************************************
 ** HL1606
 ******************************************************************************/

void write_buffer_to_strip()
{
    for(uint8_t i = 0; i < strip.numLEDs(); ++i)
    {
        strip.setLEDcolor(i, led_buffer[i]);
    }
    strip.writeStrip();     
}

void swish(uint8_t time, uint8_t top_color)
{
    swish(time, top_color, BLACK);
}

void swish(uint16_t time, uint8_t top_color, uint8_t bot_color)
{
    for(uint8_t i = 0; i < strip.numLEDs(); ++i)
    {
        bar_graph(i, top_color, bot_color);
        delay(time);  
    }
    for(uint8_t i = strip.numLEDs(); i >= 0 ; --i)
    {
        bar_graph(i, top_color, bot_color);
        delay(time);  
    }
}

void bar_graph(uint8_t val, uint8_t top_color, uint8_t bot_color)
{
    for(uint8_t i = 0; i < strip.numLEDs(); ++i)
    {
        if (i <= val) 
        {
            strip.setLEDcolor(i, top_color);   
        } else 
        {
            strip.setLEDcolor(i, bot_color);   
        }    
    }
    strip.writeStrip();     
}

void bar_graph(int val, int color)
{
    bar_graph(val, color, BLACK);
}

void slow_test()
{
   color_fill(RED);
   delay(1000);
   strip_off();
   color_fill(GREEN);
   delay(1000);
   strip_off();
   color_fill(BLUE);
   delay(1000);
   strip_off();
}

void strip_off(void) 
{
    // turn all LEDs off!
    for (uint8_t i=0; i < strip.numLEDs(); ++i)
    {
        strip.setLEDcolor(i, BLACK);
    }
    strip.writeStrip();   
}
 
// fill the entire strip, with a delay between each pixel for a 'wipe' effect
void color_wipe(uint8_t color, uint16_t wait) {
    for (uint8_t i = 0; i < strip.numLEDs(); ++i) 
    {
        strip.setLEDcolor(i, color);
        strip.writeStrip();   
        delay(wait);
    }
}

// fill the entire strip, with a delay between each pixel for a 'wipe' effect
void color_fill(uint8_t color) {
    for (uint8_t i = 0; i < strip.numLEDs(); ++i)
    {
        strip.setLEDcolor(i, color);      
    }
    strip.writeStrip();   
}

/*******************************************************************************
 ** Filesystem
 ******************************************************************************/

uint16_t get_image_count()
{
    Image fd;
    char *fn = "imgcnt.dat";

    if (!SD.exists(fn)) 
    { 
#ifdef SERIAL_DEBUG
        Serial.print("Failed to find ");
        Serial.println(fn);
#endif
        return -1; 
    }

    fd = SD.open(fn, FILE_READ);
    if (!fd)
    {
#ifdef SERIAL_DEBUG
        Serial.print("Failed to open ");
        Serial.println(fn);
#endif
        return -1; 
    }

    return static_cast<uint16_t>((fd.read() << 8) | fd.read());
}

bool write_image(uint16_t idx)
{
    File image;
    uint32_t sz;
    uint32_t ts;
    char fn[32];
    uint16_t cols;
    uint8_t *buf;

    itoa(idx, fn, 10);
    strcat(fn, ".scy");
    if (!SD.exists(fn)) 
    { 
#ifdef SERIAL_DEBUG
        Serial.print("Failed to find ");
        Serial.println(fn);
#endif
        return false; 
    }
    image = SD.open(fn, FILE_READ);
    if (!image)
    {
#ifdef SERIAL_DEBUG
        Serial.print("Failed to open ");
        Serial.println(fn);
#endif
        return false; 
    }

    sz = image.size();
    cols = sz / NUM_LEDS;

#ifdef SERIAL_DEBUG
    Serial.print("Writing ");
    Serial.print(fn);
    Serial.print(" ");
    Serial.print(cols);
    Serial.println(" columns");
#endif

    for (uint16_t col = 0; col < cols; ++col)
    {
        ts = millis();
        buf = led_buffer;
        for (uint8_t led = 0; led < NUM_LEDS; ++led)
        {
            *buf++ = image.read();
        }
        write_buffer_to_strip();
#ifdef SERIAL_DEBUG
            Serial.print("Wrote col #");
            Serial.println(col);
#endif
        scan_rate = map(analogRead(SPEED_DIAL_PIN), 0, 1023, 50, 200);
        while((millis() - ts) < scan_rate)
        {}
    }

    image.close();
    return true;
}

void update_flags(uint8_t threshold=50)
{
    int8_t counts[BUTTON_COUNT];
    for(uint8_t pin = 0; pin < BUTTON_COUNT; ++pin)
    {
        counts[pin] = 0;
    }
    for(uint8_t i = 0; i < threshold; ++i)
    {
        for(uint8_t pin = 0; pin < BUTTON_COUNT; ++pin)
        {
            counts[pin] = digitalRead(_pin_map[pin]) ? (counts[pin] + 1) : (counts[pin] - 1);
        }
    }
    for(uint8_t pin = 0; pin < BUTTON_COUNT; ++pin)
    {
        button_state[pin] = counts[pin] > 0;
    }
}

/*******************************************************************************
 ** setup / loop
 ******************************************************************************/

void setup(void) 
{
    for(uint8_t pin = 0; pin < BUTTON_COUNT; ++pin)
    {
        pinMode(_pin_map[pin], OUTPUT);
    }
    
    color_fill(_RED);
    delay(1000);
    color_fill(_GRN);
    delay(1000);
    color_fill(_BLU);
    delay(1000);
    color_fill(BLACK);

#ifdef SERIAL_DEBUG
    Serial.begin(9600);
    Serial.println("LightScythe v.02");
#endif
    SD.begin(CS_PIN);
    ImageCount = get_image_count();
    CurrentImage = 0;
}

void loop(void) 
{ 
    update_flags();

    if ((!button_state[ONE_SHOT_IDX]) || (button_state[ONE_SHOT_IDX] && button_state[GO_IDX]))
    {
        write_image(current_image);
    }

    if (button_state[AUTO_INC_IDX] || button_state[UP_IDX])
    {
        current_image = (current_image + 1) % image_count;
    } else
    if (button_state[DOWN_IDX])
    {
        current_image = (current_image - 1) % image_count;
    }
}

