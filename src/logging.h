#ifndef LOGGING_H
#define LOGGING_H

// Ucomment LOGGING_UART enable logging messages via the UART
#define LOGGING_UART

// If logging is disabled, then the functions are noops

#ifdef LOGGING_UART
#include <stdio.h>
#define logging_init() stdio_init_all()
#define logf(...) printf(__VA_ARGS__)
#else
#define logging_init()
#define logf(...)
#endif

#endif
