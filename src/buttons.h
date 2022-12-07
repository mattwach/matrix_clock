#ifndef BUTTONS_H
#define BUTTONS_H
// Manages button state

#include <inttypes.h>

// These define bits in the return array.  Sequence should be 1, 2, 4, 8, ...
#define SELECT_BUTTON 1
#define INCREMENT_BUTTON 2

void buttons_init(void);

// returns current button state as a bit array
uint8_t buttons_get(void);

#endif
