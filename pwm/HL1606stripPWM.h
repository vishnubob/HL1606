#ifndef __HL1606_STRIP_PWM_
#define __HL1606_STRIP_PWM_
// (c) Adafruit Industries / Limor Fried 2010. Released under MIT license.

#include <WProgram.h>

// some spi defines
// Classic Arduinos
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__)|| defined(__AVR_ATmega168__) 
    #define SPI_PORT    PORTB
    #define SPI_DDR     DDRB
    #define SPI_PIN     PINB
    #define SPI_MOSI    3       // Arduino pin 11.
    #define SPI_SCK     5       // Arduino pin 13.
    #define DATA_PIN    11
    #define CLOCK_PIN   13
// Megas
#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    #define SPI_PORT    PORTB
    #define SPI_DDR     DDRB
    #define SPI_PIN     PINB
    #define SPI_MOSI    2       // Arduino pin 51.
    #define SPI_SCK     1       // Arduino pin 52.
    #define DATA_PIN    51
    #define CLOCK_PIN   52
#elif defined(__AVR_ATmega32U4__)
    #define SPI_PORT    PORTB
    #define SPI_DDR     DDRB
    #define SPI_PIN     PINB
    #define SPI_MOSI    2       // Teensyduino pin 2
    #define SPI_SCK     1       // Teensyduino pin 1
    #define DATA_PIN    2
    #define CLOCK_PIN   1
#endif

class HL1606stripPWM {
private:
    uint8_t _bitdepth;
    uint8_t _spi_div;

    void _timer_init(void);
    void _spi_init(void);

public:
    HL1606stripPWM(uint8_t length, uint8_t latch);
    void begin(void);
    void report(void);

    uint8_t get_length(void)
    { return _length; }

    uint8_t get_bitdepth()
    { return _bitdepth; }
    void set_bitdepth(uint8_t b) 
    {
        _bitdepth = b;
        pwmincr = 256 / (1 << _bitdepth);
    }

    uint8_t get_div()
    { return _spi_div; }
    void set_div(uint8_t div);

    uint8_t getCPUmax()
    { return CPUmaxpercent; } 
    void setCPUmax(uint8_t cpumax)
    {
        CPUmaxpercent = cpumax;
        _timer_init();
    }

    uint8_t get_level(uint8_t level)
    {
        return min(0xff, (256u / (1 << _bitdepth)) * level);
    }

    void clear()
    {
        for (int i = 0; i < _length; ++i)
        {
             redPWM[i] = 0;
             greenPWM[i] = 0;
             bluePWM[i] = 0;
        }
    }

    void fill_color(uint8_t r, uint8_t g, uint8_t b)
    {
        for (int i = 0; i < _length; ++i)
        {
             redPWM[i] = r;
             greenPWM[i] = g;
             bluePWM[i] = b;
        }
    }

    void get_color(uint8_t n, uint8_t *r, uint8_t *g, uint8_t *b) 
    {
         if (n >= _length)
            return;
         *r = redPWM[n];
         *g = greenPWM[n];
         *b = bluePWM[n];
    }

    void set_color(uint8_t n, uint8_t r, uint8_t g, uint8_t b) 
    {
         redPWM[n] = r; 
         greenPWM[n] = g; 
         bluePWM[n] = b;
    }

    void add_color(uint8_t n, uint8_t r, uint8_t g, uint8_t b) 
    {
         redPWM[n] = max(0, min(0xff, ((uint16_t)redPWM[n]) + r));
         greenPWM[n] = max(0, min(0xff, ((uint16_t)greenPWM[n]) + g));
         bluePWM[n] = max(0, min(0xff, ((uint16_t)bluePWM[n]) + b));
    }

public:
    uint8_t     CPUmaxpercent;
    uint8_t     pwmincr;
    uint8_t     pwmcounter;
    uint8_t     *redPWM;
    uint8_t     *greenPWM;
    uint8_t     *bluePWM;
    uint8_t     _length;
    uint8_t     _latch_pin;
};

#endif // __HL1606_STRIP_PWM_
