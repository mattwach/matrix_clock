// some debugging routine that you might find useful
#ifndef MATRIX_CLOCK_DEBUG_H
#define MATRIX_CLOCK_DEBUG_H

#include <inttypes.h>

void debug_printf(const char* fmt, ...);

// dump led state to stdout
void debug_dump_led(const uint32_t* led);

// wait for a keypress
void debug_wait_for_key(void);

// convienence
void debug_dump_led_with_wait(const char* msg, const uint32_t* led);

#endif

