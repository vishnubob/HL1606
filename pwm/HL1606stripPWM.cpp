#include "HL1606stripPWM.h"

static HL1606stripPWM *__strip;

HL1606stripPWM::HL1606stripPWM(uint8_t l, uint8_t n) 
{
    _length = 0;
    _latch_pin = l;
    pwmcounter = 0;
    _bitdepth = 3;
    CPUmaxpercent = 70;
    pwmincr = 256 / (1 << _bitdepth);
    _spi_div = 32;

    if (_length)
    {
        redPWM = (uint8_t *)malloc(_length);
        greenPWM = (uint8_t *)malloc(_length);
        bluePWM = (uint8_t *)malloc(_length);
    } else
    {
        redPWM = 0;
        greenPWM = 0;
        bluePWM = 0;
    }

    for (uint8_t i = 0; i < _length; ++i) 
    {
        set_color(i, 0, 0, 0);
    }
}

// XXX: Don't do this more then once
// memory will fragment
void HL1606stripPWM::set_length(uint8_t n)
{
    if(_length)
    {
        free(redPWM);
        free(greenPWM);
        free(bluePWM);
    }

    _length = n;
    redPWM = (uint8_t *)malloc(_length);
    greenPWM = (uint8_t *)malloc(_length);
    bluePWM = (uint8_t *)malloc(_length);

    for (uint8_t i = 0; i < _length; ++i) 
    {
        set_color(i, 0, 0, 0);
    }
}

void HL1606stripPWM::begin(void) 
{
    _spi_init();
    _timer_init();
    __strip = this;

    // run our strip-writing interrupt
#if defined(__AVR_ATmega32U4__) 
    TIMSK3 = _BV(OCIE3A);
#else
    TIMSK2 = _BV(OCIE2A);
#endif
}

void HL1606stripPWM::_timer_init(void)
{
    // calculate how long it will take to pulse one strip down
    double time = _length;    // each LED
    time *= 8;              // 8 bits of data for each LED;
    time *= _spi_div;    // larger dividers = more time per bit
    time *= 1000;           // time in milliseconds
    time /= F_CPU;          // multiplied by how long it takes for one instruction (nverse of cpu)

    time *= 100;            // calculate what percentage of CPU we can use (multiply by time)
    time /= CPUmaxpercent;

    // set up the interrupt to write to the entire strip
    // Each pixel requires a 1-byte SPI transfer, so with a n-pixel strip, thats n bytes at 1 MHz
    // which makes for a 1 MHz / n PWM frequency. Say for a 100 LED strip, we can update no faster
    // than 10 KHz. Lets make it 1 KHz to start

#if defined(__AVR_ATmega32U4__) 
    // for the atmega32u4 we'll use counter #3
    TCCR3A = 0;
    TCCR3B = _BV(CS32) | _BV(WGM32);  // CTC mode, /256 clock
    OCR3A = (F_CPU/256) / (1000 / time);
    TCNT3 = 0;
#else
    // we'll use timer 2 in CTC mode
    TCCR2A = _BV(WGM21);     // CTC mode
    TCCR2B = _BV(CS21) | _BV(CS22);   // 256 divider, run timer2 at 62.5 KHz (16MHz/256)
    OCR2A = (F_CPU/256) / (1000 / time);
    TCNT2 = 0;
#endif
} 

#if defined(__AVR_ATmega32U4__) 
ISR(TIMER3_COMPA_vect)
#else
ISR(TIMER2_COMPA_vect)
#endif
{
    uint8_t i, d;

    // write out data to strip 
    for (i=0; i< __strip->_length; i++) 
    {
        d = 0x80;          // set the latch bit
        // calculate the next LED's byte
        if (__strip->pwmcounter < __strip->redPWM[i]) 
        {
            d |= 0x04;
        } 
        if (__strip->pwmcounter < __strip->greenPWM[i]) 
        {
            d |= 0x10;
        } 
        if (__strip->pwmcounter < __strip->bluePWM[i]) 
        {
            d |= 0x01;
        } 

        // check that previous xfer completed
        while(!(SPSR & _BV(SPIF))); 
        // send new data
        SPDR = d; 
    }

    // increment our PWM counter
    __strip->pwmcounter += __strip->pwmincr;

    // make sure we're all done
    while(!(SPSR & _BV(SPIF)));

    // latch
    digitalWrite(__strip->_latch_pin, HIGH);
    delayMicroseconds(3);
    digitalWrite(__strip->_latch_pin, LOW);
}


void HL1606stripPWM::_spi_init(void) 
{
    pinMode(DATA_PIN, OUTPUT);
    pinMode(CLOCK_PIN, OUTPUT);
    pinMode(_latch_pin, OUTPUT);

    // enable SPI master mode
    SPCR = _BV(SPE) | _BV(MSTR);   
    // SPI clock is FCPU/32 = 500 Khz for most arduinos
    set_div(_spi_div);          
    // we send a fake SPI byte to get the 'finished' bit set in the register, dont remove!!!
    SPDR = 0;
}

void HL1606stripPWM::set_div(uint8_t spi_div) {
    _spi_div = spi_div;

    switch (spi_div) 
    {
        case 2:
            SPSR |= _BV(SPI2X);
            break;
        case 4:
            // no bits set
            break;
        case 8:
            SPCR |= _BV(SPR0); 
            SPSR |= _BV(SPI2X);
            break;
        case 16:
            SPCR |= _BV(SPR0); 
            break;
        case 32:
            SPCR |= _BV(SPR1); 
            SPSR |= _BV(SPI2X);
            break;
        case 64:
            SPCR |= _BV(SPR1); 
            break;
        default:
            // slowest
            case 128:
            SPCR |= _BV(SPR1);
            SPCR |= _BV(SPR0);  
    }
    _timer_init();
}

void HL1606stripPWM::report()
{
    double time = _length;
    // 8 bits of data for each LED;
    time *= 8;
    // larger dividers = more time per bit
    time *= _spi_div;
    // time in milliseconds
    time *= 1000;
    // multiplied by how long it takes for one instruction (nverse of cpu)
    time /= F_CPU;

    Serial.print("Time per LED strip write: "); 
    Serial.print(time); 
    Serial.println(" millis");

    time *= 100;
    time /= get_length();

    Serial.print("Time allowed per interrupt ");
    Serial.print(time);
    Serial.println(" millis");

    // get that in Hz
    time = 1000 / time;
    Serial.print("Interrupt freq: "); 
    Serial.print(time);
    Serial.println(" Hz");

    // Now find the final 'color cycle' frequency
    Serial.print("Bitdepth: ");
    Serial.println(1 << get_bitdepth());
    Serial.print("Color cycle freq: "); 
    Serial.print(time / (1 << get_bitdepth()));
    Serial.println(" Hz");
    // Try to keep the color frequency above 60 Hz (flickery). 
    // 100 Hz or higher looks good to most people
}

