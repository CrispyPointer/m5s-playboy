#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_freertos_hooks.h"
#include "freertos/semphr.h"
#include "esp_system.h"

#include "m5s_button.h"
#include "tiny_timer.h"
#include "driver/gpio.h"

BUTTON_DATA_T button_data;
#define DEBOUNCE_DELAY 10 //ms

void m5s_button_init(uint8_t* gpio)
{

    printf("Button init\n");

    for (int i = 0; i < BUTTON_NUM; i++)
    {
        button_data.gpio[i] = gpio[i];
        button_data.state[i] = 1u; // The pins are pulled up
        button_data.prev_state[i] = 1u;
    }

    button_data.debounce_delay = DEBOUNCE_DELAY;

    // Config GPIO no-pull from button data
    for (int i = 0; i < BUTTON_NUM; i++)
    {
        gpio_config_t io_conf;
        io_conf.intr_type = GPIO_INTR_DISABLE;
        io_conf.mode = GPIO_MODE_INPUT;
        io_conf.pin_bit_mask = (1ULL << button_data.gpio[i]);
        io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
        io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
        gpio_config(&io_conf);
    }
}

/** Update button state in non-blocking mode */
static void m5s_button_update(void)
{
    while (true)
    {
        for (int i = 0; i < BUTTON_NUM; i++)
        {
            button_data.state[i] = gpio_get_level(button_data.gpio[i]);
            if (button_data.state[i] != button_data.prev_state[i])
            {
                button_data.changed = false;
                if (timer_get_elapsed_time(button_data.timer[i]) > button_data.debounce_delay)
                {
                    button_data.changed = true;
                    button_data.prev_state[i] = button_data.state[i];
                    timer_reset_time(&button_data.timer[i]);
                }
            }
        }

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

static bool m5s_button_is_valid(uint8_t index)
{
    return (index != BUTTON_A && index != BUTTON_B && index != BUTTON_C) ? false : true;
}

static uint8_t button_to_index(uint8_t button)
{
    uint8_t index = UINT8_MAX;
    switch (button)
    {
    case BUTTON_A:
        index = 0u;
        break;
    case BUTTON_B:
        index = 1u;
        break;
    case BUTTON_C:
        index = 2u;
        break;
    default:
        // Do nothing
        break;
    }
    return index;
}

static bool m5s_button_get_past_state(uint8_t index)
{
    assert(index < BUTTON_NUM);

    if (button_data.changed)
    {
        return button_data.state[index] == 0u ? false : true;
    }
    else
    {
        return false;
    }
}

static bool m5s_button_get_state(uint8_t index)
{
    assert(index < BUTTON_NUM);
    return button_data.state[index] == 0u ? true : false;
}

bool m5s_button_is_pressed(uint8_t index)
{
    return m5s_button_get_state(index);
}

bool m5s_button_was_pressed(uint8_t index)
{
    return m5s_button_get_past_state(index);
}

uint32_t m5s_button_get_timer_ms(uint8_t index)
{
    return timer_get_elapsed_time(button_data.timer[index]);
}

uint32_t m5s_button_get_timer_s(uint8_t index)
{
    return timer_get_elapsed_time(button_data.timer[index]) / 1000u; // Convert to seconds
}

void m5s_button_task_init(void)
{
    uint8_t gpio[BUTTON_NUM] = {BUTTON_A, BUTTON_B, BUTTON_C};
    m5s_button_init(gpio);
    xTaskCreate((TaskFunction_t)m5s_button_update, "m5s_button_init", 1024, NULL, 3, NULL);
}
