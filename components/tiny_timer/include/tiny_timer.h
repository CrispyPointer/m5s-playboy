#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

void timer_reset_time(uint32_t* timer);
uint32_t timer_get_elapsed_time(uint32_t timer);

#endif // TIMER_H
