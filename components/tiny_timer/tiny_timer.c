#include <stdio.h>
#include "tiny_timer.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void timer_reset_time(uint32_t* timer)
{
    *timer = xTaskGetTickCount() * portTICK_PERIOD_MS;
}

uint32_t timer_get_elapsed_time(uint32_t timer)
{
    return ((xTaskGetTickCount()* portTICK_PERIOD_MS) - timer);
}

void timer_delay_ms(uint32_t delay)
{
    vTaskDelay(delay / portTICK_PERIOD_MS);
}