#ifndef CLOCK_H
#define CLOCK_H

// Generic clock header.
//
// The currently-implemented .c file talks to a DS3231 RTC module but
// you can use other modules, an atomic clock receiver, a gps module,
// internet itme or whatever you want on the backend - it just has to
// provide the following functions as-declared below.  Thhen just
// link in the new implementation in CMakeLists.txt

#include "pico/stdlib.h"

// gets the current time in HHMM format (0000 - 2359)
// For example, if 1354 is returned, the time is 13:54 (or
// 1:34 PM if you prefer)
uint16_t clock_get_time();

// Sets the time.  For example: time_hhmm = 1234 would set the
// time to 12:34.  Note that 24h time is always used so 1234
// is 12:34 PM and 0034 is 12:34 AM.
void clock_set_time(uint16_t time_hhmm);

// initialize the clock
void clock_init();

#endif
