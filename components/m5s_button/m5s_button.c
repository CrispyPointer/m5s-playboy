#include <stdio.h>
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
void m5s_button_update(void)
{
    for (int i = 0; i < BUTTON_NUM; i++)
    {
        timer_reset_time(&button_data.timer[i]);
        button_data.state[i] = gpio_get_level(button_data.gpio[i]);
        if (button_data.state[i] != button_data.prev_state[i])
        {
            if (timer_get_elapsed_time(button_data.state[i]) > button_data.debounce_delay)
            {
                button_data.prev_state[i] = button_data.state[i];
            }
        }
    }
}

bool m5s_button_get_state(uint8_t button)
{
    bool state = false;
    if (button == BUTTON_A)
    {
        state = button_data.state[0u] == 0u ? true : false;
    }
    else if (button == BUTTON_B)
    {
        state = button_data.state[1u] == 0u ? true : false;
    }
    else if (button == BUTTON_C)
    {
        state = button_data.state[2u] == 0u ? true : false;
    }
    else
    {
        // Do nothing
    }

    return state;
}