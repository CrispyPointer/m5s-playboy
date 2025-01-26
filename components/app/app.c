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

APP_DATA_T app_data = {
    .page = UNKNOWN_PAGE,
    .menu_index = 0u,
    .menu_items = {"New game", "Load", "Settings", "Exit"},
};

static void app_menu_fetch(void)
{
    for (uint8_t i = 0; i < MENU_ITEM_NUM; i++)
    {
        // display(page, menu, index)
    }
}

static void app_init(void)
{
    app_data.page = MAIN_MENU;
}

static void app_proc(void)
{
}
