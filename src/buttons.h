#ifndef BUTTONS_H
#define BUTTONS_H
// Manages button state

#include <inttypes.h>

// These define bits in the return array.  Sequence should be 1, 2, 4, 8, ...
#define SELECT_BUTTON 1
#define INCREMENT_BUTTON 2

void buttons_init(void);

// returns current button state as a bit array
// Note that reading the value also resets the state back to unpressed
// e.g.
//  * somewone presses and releases the SELECT_BUTTON
//  * some amount of time goes by
//  * buttons_get() is called
//  * SELECT_BUTTON is returned
//  * any future calls to buttons_get() return zero until a button
//    is pressed again.
uint8_t buttons_get(void);

// returns a 1 if there is a recent current overload event.
uint8_t is_current_over(void);

#endif
