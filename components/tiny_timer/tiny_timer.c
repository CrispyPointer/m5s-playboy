#include <stdio.h>
#include "tiny_timer.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void timer_reset_time(uint32_t* timer)
{
    *timer = xTaskGetTickCount();
}

uint32_t timer_get_elapsed_time(uint32_t timer)
{
    return xTaskGetTickCount() - timer;
}
