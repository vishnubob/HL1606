#ifndef __HSV_TO_RGB__
#define __HSV_TO_RGB__
#include "config.h"

void hsv2rgb(int16_t h, int16_t s, int16_t v, uint8_t *r, uint8_t *g, uint8_t *b)
{
    int f, p, q, t;
    while( h < 0 ) h += 3600;
    while( h >= 3600 ) h-= 3600;
    f = h%600;
    p = ( v * ( 255 - s ) ) / ( 255 );
    q = ( v * ( 600*255 - f*s ) ) / ( 255 * 600 );
    t = ( v * ( 600*255 - ( 600 - f ) * s ) ) / ( 255 * 600 );

    if( s == 0 ) /* gray */
    {
        *r = v;
        *g = *r;
        *b = *r;
        return;
    }

    switch( h/600 )
    {
        case 0:
            *r = v;
            *g = t;
            *b = p;
            break;
        case 1:
            *r = q;
            *g = v;
            *b = p;
            break;
        case 2:
            *r = p;
            *g = v;
            *b = t;
            break;
        case 3:
            *r = p;
            *g = q;
            *b = v;
            break;
        case 4:
            *r = t;
            *g = p;
            *b = v;
            break;
        case 5:
            *r = v;
            *g = p;
            *b = q;
            break;
    }
}
#endif // __HSV_TO_RGB__
