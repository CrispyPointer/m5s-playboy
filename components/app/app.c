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
#include "m5s_button.h"
#include "display.h"
#include "power.h"
#include "tiny_timer.h"
#include "tetris.h"

typedef struct
{
    bool page_switched;
    PAGE_T page;
    uint8_t menu_index;
    const char* menu_items[MENU_ITEM_NUM];
    bool btn_pressed[BUTTON_NUM];
    bool btn_past_pressed[BUTTON_NUM];
} APP_DATA_T;

APP_DATA_T app_data = {
    .page = UNKNOWN_PAGE,
    .menu_index = 0u,
    .menu_items = {"New game", "Device", "Settings", "Exit"},
};

static void app_menu_display(uint8_t menu_index, bool reset)
{
    static uint32_t prev_index = UINT32_MAX;
    if (reset)
    {
        display_draw_menu(app_data.menu_items, MENU_ITEM_NUM, menu_index);
    }
    else if (prev_index != menu_index)
    {
        display_draw_menu(app_data.menu_items, MENU_ITEM_NUM, menu_index);
        prev_index = menu_index;
    }
    else
    {
        // Do nothing
    }
}

static void app_init(void)
{
    app_data.page = MENU_PAGE;
    display_init();
    app_menu_display(app_data.menu_index, false);
}

static void app_device_display(bool reset)
{
    static uint32_t prev_soc = UINT32_MAX;
    const uint32_t soc = power_get_batt_soc();
    if (reset)
    {
        display_draw_device(soc, 0u, 0u);
    }
    else if (prev_soc != soc)
    {
        display_draw_device(soc, 0u, 0u);
        prev_soc = soc;
    }
    else
    {
        // Do nothing
    }
}

static void app_handle_button_menu(void)
{
    static bool btn_a_cache, btn_b_cache, btn_c_cache;

    if (app_data.btn_pressed[BUTTON_A_INDEX] && !btn_a_cache && !app_data.page_switched)
    {
        // Move down the menu
        if (app_data.menu_index < (MENU_ITEM_NUM - 1u))
        {
            app_data.menu_index++;
        }
        btn_a_cache = true;
    }
    else if (app_data.btn_past_pressed[BUTTON_A_INDEX])
    {
        btn_a_cache = false;
        app_data.page_switched = false; // Reset page switch flag otherwise it will be stuck in this index
    }

    if (app_data.btn_pressed[BUTTON_B_INDEX] && !btn_b_cache)
    {
        // Move up the menu
        if (app_data.menu_index != 0u)
        {
            app_data.menu_index--;
        }
        btn_b_cache = true;
    }
    else if (app_data.btn_past_pressed[BUTTON_B_INDEX])
    {
        btn_b_cache = false;
    }

    if (app_data.btn_pressed[BUTTON_C_INDEX] && !btn_c_cache)
    {
        btn_c_cache = true;
        // Change page index
        switch (app_data.menu_index)
        {
        case NEW_GAME:
            app_data.page = NEW_GAME_PAGE;
            tetris_start();
            app_data.page_switched = true;
            break;
        case DEVICE:
            app_data.page = DEVICE_PAGE;
            app_data.page_switched = true;
            app_device_display(true); // First time enable
            break;
        case EXIT:
            power_shut_down();
            break;
        default:
            break;
        }
    }
    else if (app_data.btn_past_pressed[BUTTON_C_INDEX])
    {
        btn_c_cache = false;
    }
}

static void app_handle_button_device(void)
{
    if (app_data.btn_pressed[BUTTON_A_INDEX])
    {
        app_data.page = MENU_PAGE;
        app_data.menu_index = NEW_GAME;
    }
}

static void app_fetch_button(void)
{
    for (uint8_t i = 0; i < BUTTON_NUM; i++)
    {
        app_data.btn_pressed[i] = m5s_button_is_pressed(i);
        app_data.btn_past_pressed[i] = m5s_button_was_pressed(i);
    }
}

static void app_handle_page(PAGE_T page)
{
    app_fetch_button();

    switch (page)
    {
    case MENU_PAGE:
        app_handle_button_menu();
        app_menu_display(app_data.menu_index, false);
        break;
    case NEW_GAME_PAGE:
        // Handle new game page
        const bool game_end = tetris_is_ended();
        if (game_end)
        {
            app_data.page = MENU_PAGE;
            app_menu_display(app_data.menu_index, true); // Reset menu page
        }
        break;
    case DEVICE_PAGE:
        app_handle_button_device();
        app_device_display(false);
        break;
    case SETTINGS_PAGE:
        // Handle settings page
        break;
    case UNKNOWN_PAGE:
        // Handle unknown page
        break;
    default:
        // Handle default
        break;
    }
}

static void app_proc(void)
{
    while (true)
    {
        app_handle_page(app_data.page);
        timer_delay_ms(25);
    }
}

void app_task_init(void)
{
    app_init();
    xTaskCreate((TaskFunction_t)app_proc, "app_init", 4096, NULL, 3, NULL);
}
