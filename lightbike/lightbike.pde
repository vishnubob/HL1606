#include "config.h"
#include "Thing.h"
#include "hsv2rgb.h"

int latchPin = 10;
HL1606stripPWM strip = HL1606stripPWM(32, latchPin); 

void setup() 
{
    Serial.begin(9600);
    Serial.println("hello!");
    strip.set_bitdepth(4);
    strip.set_div(16);
    strip.setCPUmax(70);
    strip.begin();
    strip.report();
    randomSeed(analogRead(0));
}

void loop()
{
    uint8_t last_mode = -1;
    const uint8_t total_modes = 3;
    while(1)
    {
        uint8_t next_mode = last_mode;
        while (last_mode == next_mode)
        {
            next_mode = random(total_modes);
        }
        //switch(next_mode)
        switch(0)
        {
            case 0:
                rainbow();
                break;
            case 1:
                spaz_pulse();
                break;
            case 2:
                crazy_color_throb();
                break;
            case 3:
                crazy_fun_time();
                break;
        }
        last_mode = next_mode;
    }
}

/*******************************************************************************
 * Effects
 ******************************************************************************/

void throb(uint16_t dtime, uint8_t colors)
{
    uint8_t bitdepth = (1 << strip.get_bitdepth());
    uint8_t step = 256 / bitdepth;
    for(int16_t b = 0; b <= 0xff; b += step)
    {
        strip.fill_color(colors & RED ? b : 0, 
                            colors & GRN ? b : 0, 
                            colors & BLU ? b : 0);
        delay(dtime);
    }
    strip.fill_color(colors & RED ? 0xff : 0, 
                        colors & GRN ? 0xff : 0, 
                        colors & BLU ? 0xff : 0);
    for(int16_t b = 0xff; b >= 0; b -= step)
    {
        strip.fill_color(colors & RED ? b : 0, 
                            colors & GRN ? b : 0, 
                            colors & BLU ? b : 0);
        delay(dtime);
    }
    strip.fill_color(0, 0, 0);
    delay(dtime);
}

/*******************************************************************************
 * Modes
 ******************************************************************************/

void crazy_color_throb()
{
    uint16_t dec = random(10, 20);
    int16_t dtime = random(50, 200);
    while(dtime > 20)
    {
        throb(dtime, 1 + random(7));
        dtime -= dec;
    }
    for(uint8_t i = random(20); i > 0; --i)
    {
        throb(random(5, 15), 1 + random(7));
    }

}

void crazy_fun_time()
{
    UltraThing things[5];
    uint16_t dstep = random(10, 70);
    uint8_t thing_cnt = random(2, 6);
    for(uint8_t i = 0; i < thing_cnt; ++i)
    {
        things[i].rand_init(&strip);
    }

    for(uint16_t i = 2000; i > 0; --i)
    {
        bool all_dead = true;
        for(uint8_t i = 0; i < thing_cnt; ++i)
        {
            things[i].draw();
            all_dead &= things[i].is_dead();
        }
        if(all_dead)
            break;
        delay(dstep);
        strip.clear();
    }
}


void spaz_pulse()
{
    Thing things[5];
    uint16_t dstep = random(10, 70);
    uint8_t thing_cnt = random(2, 6);
    for(uint8_t i = 0; i < thing_cnt; ++i)
    {
        things[i].rand_init(&strip);
    }

    for(uint16_t i = 2000; i > 0; --i)
    {
        bool all_dead = true;
        for(uint8_t i = 0; i < thing_cnt; ++i)
        {
            things[i].draw();
            all_dead &= things[i].is_dead();
        }
        if(all_dead)
            break;
        delay(dstep);
        strip.clear();
    }
}

void rainbow() 
{
    uint8_t dtime = random(5);
    int16_t s = 1;
    int16_t v = 1;
    for(int8_t reps = random(1, 5); reps > 0; --reps)
    {
        for(uint16_t hue = 0; hue < 3600; hue += 36)
        {
            for (uint8_t i = 0; i < strip.get_length() ; i++) 
            {
                int16_t h = (hue + i * 36) % 3600;
                uint8_t r, g, b;
                hsv2rgb(h, s, v, &r, &g, &b);
                strip.set_color(i, r, g, b);
            }
            delay(dtime);
        }
        s += random(5);
        v += random(5);
    }
}

