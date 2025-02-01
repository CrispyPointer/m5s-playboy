/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_freertos_hooks.h"
#include "freertos/semphr.h"
#include "esp_system.h"

#include "app.h"
#include "defines.h"
#include "M5Unified.h"
#include "m5s_button.h"
#include "display.h"

extern "C" void app_main(void)
{
    M5.begin(); // M5STACK INITIALIZE
    M5.Power.begin();

    m5s_button_task_init();
    app_task_init();
}
