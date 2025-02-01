#ifndef M5S_BUTTON_H
#define M5S_BUTTON_H

#include <stdint.h>
#include <stdbool.h>
#include "defines.h"

typedef struct
{
    bool changed;
    uint8_t gpio[BUTTON_NUM];
    uint8_t state[BUTTON_NUM];
    uint8_t prev_state[BUTTON_NUM];
    uint32_t timer[BUTTON_NUM];
    uint32_t debounce_delay;
} BUTTON_DATA_T;

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Initialize button task
     */
    void m5s_button_task_init(void);

    /**
     * @brief Get button state
     * @param index Button index
     */
    bool m5s_button_is_pressed(uint8_t index);

    /**
     * @brief Get button previous state
     * @param index Button index
     */
    bool m5s_button_was_pressed(uint8_t index);

    /**
     * @brief Get button timer in milliseconds
     * @param index Button index
     * @return Button timer in milliseconds
     */
    uint32_t m5s_button_get_timer_ms(uint8_t index);

    /**
     * @brief Get button timer in seconds
     * @param index Button index
     * @return Button timer in seconds
     */
    uint32_t m5s_button_get_timer_s(uint8_t index);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // M5S_BUTTON_H
