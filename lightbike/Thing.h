#ifndef __THING_H
#define __THING_H

#include "config.h"
#include "HL1606stripPWM.h"

class Thing
{
protected:
    float _pos;
    float _speed;
    float _accel;
    int8_t _trail[10];
    uint8_t _trail_len;
    uint8_t _trail_idx;
    uint8_t _head_color;
    uint8_t _tail_color;
    uint8_t _clip_count;
    HL1606stripPWM *_strip;

    void _init_trail()
    {
        for(uint8_t i = 0; i < _trail_len; ++i)
        {
            _trail[i] = -1;
        }
    }

public:
    Thing(HL1606stripPWM *strip = NULL) : _trail_idx(0), _clip_count(0), _strip(strip)
    {}

    Thing(int8_t pos, float speed, float accel, uint8_t trail_len=0, uint8_t head_color=0, uint8_t tail_color=0) :
        _pos(pos), _speed(speed), _trail_len(trail_len), _trail_idx(0),
        _head_color(head_color), _tail_color(tail_color), _accel(accel),
        _clip_count(0), _strip(0)
    { _trail_len = min(trail_len, 10); _init_trail(); }

    void rand_init(HL1606stripPWM *strip)
    {
        _strip = strip;
        _pos = random(_strip->get_length());
        _speed = random(100, 1500) / 1000.0 * (random(1) ? -1 : 1);
        _accel = random(100, 500) / 1000.0 * (random(1) ? -1 : 1);
        _trail_len = random(10);
        _head_color = random(1, 8);
        _tail_color = random(1, 8);
    }

    void push_trail(uint8_t pos)
    {
        if (!_trail_len) return;
        uint8_t _write_idx = (_trail_idx + _trail_len) % _trail_len;
        _trail[_write_idx] = _pos;
        _trail_idx = (_trail_idx + 1) % _trail_len;
    }

    uint8_t get_trail(uint8_t offset)
    {
        if (!_trail_len) return 0;
        return _trail[(offset + _trail_idx) % _trail_len];
    }

    bool step()
    {
        _pos += _speed;
        _speed += _accel;
        if(abs(_speed) > 4)
        {
            _accel *= -1;
        }
        if (is_clipped())
        {
            _speed *= -1;
            _clip_count += 1;
            return false;
        } else
        {
            _clip_count = 0;
            return true;
        }
    }

    bool is_dead()
    {
        return _clip_count > 5;
    }

    void draw()
    {
        if (is_dead()) 
            return;
        if (step())
        {
            _strip->add_color(_pos,
                            _tail_color & RED ? 0xff : 0,
                            _tail_color & GRN ? 0xff : 0,
                            _tail_color & BLU ? 0xff : 0);
            for(uint8_t i = 0; i < _trail_len; ++i)
            {
                _strip->add_color(get_trail(i), 
                                _tail_color & RED ? _strip->get_level(i + 1) : 0,
                                _tail_color & GRN ? _strip->get_level(i + 1) : 0,
                                _tail_color & BLU ? _strip->get_level(i + 1) : 0);
            }
            push_trail(_pos);
        }
    }

    bool is_clipped()
    {
        return ((_pos < 0) || (_pos >= _strip->get_length()));
    }
};

class UltraThing : public Thing
{
public:
    bool is_clipped()
    {
        uint16_t _rgb_flags = 0;
        uint8_t r, g, b;
        _strip->get_color(_pos, &r, &g, &b);
        bool is_filled = r | g | b;
        return ((_pos < 0) || (_pos >= _strip->get_length()) || is_filled);
    }

    bool is_dead()
    {
        if (_clip_count)
        {
            _head_color++;
            _tail_color++;
        }
        return _clip_count > 5;
    }
};


#endif // __THING_H
