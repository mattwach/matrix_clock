#ifndef DOTSTAR_CLOCK_CLOCK_H
#define DOTSTAR_CLOCK_CLOCK_H

#include "pico/stdlib.h"

// gets the current time in HHMM format (0000 - 2359)
uint16_t clock_get_time();

// sets the current time.
void clock_set_time(uint16_t time_hhmm);

// initialize i2c
void clock_init();

#endif
