#ifndef M5S_BUTTON_H
#define M5S_BUTTON_H

#include <stdint.h>
#include <stdbool.h>
#include "defines.h"

typedef struct
{
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

    void m5s_button_init(uint8_t* gpio);

    void m5s_button_update(void);

    bool m5s_button_get_state(uint8_t button);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // M5S_BUTTON_H
