#ifndef TINY_TIMER_H
#define TINY_TIMER_H

#include <stdint.h>

/**
 * @brief Reset the timer to the current time.
 * @param timer The timer to reset.
 */
void timer_reset_time(uint32_t* timer);

/**
 * @brief Get the elapsed time in milliseconds since the timer was last reset.
 * @param timer The timer to get the elapsed time from.
 * @return The elapsed time in milliseconds.
 */
uint32_t timer_get_elapsed_time(uint32_t timer);

/**
 * @brief Delay for a given number of milliseconds.
 */
void timer_delay_ms(uint32_t delay);

#endif // TINY_TIMER_H
