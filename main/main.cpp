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

#include "tiny_timer.h"
#include "m5s_button.h"
#include "defines.h"

#include "M5Unified.h"

extern "C" void app_main(void)
{
    uint8_t gpio[BUTTON_NUM] = {BUTTON_A, BUTTON_B, BUTTON_C};
    m5s_button_init(gpio);

    M5.begin(); // M5STACK INITIALIZE
    M5.Power.begin();
    M5.Lcd.setBrightness(10); // BRIGHTNESS = MAX 255

    M5.Lcd.setTextSize(1.5);
    LGFX_Sprite sprite(&M5.Lcd);
    sprite.createSprite(320, 240); // Create a sprite
    M5.Lcd.fillScreen(BLACK);   // Set background color to black
    M5.Lcd.setTextColor(WHITE); // Set text color to white

    int menuIndex = 0;
    const char* menuItems[] = {"Show battery", "Button test", "Exit"};
    const int menuSize = sizeof(menuItems) / sizeof(menuItems[0]);

    while (true)
    {
        sprite.fillScreen(BLACK); // Clear the sprite
        for (int i = 0; i < menuSize; ++i)
        {
            sprite.setCursor(10, 30 * i);
            if (i == menuIndex)
            {
                sprite.printf("> %s", menuItems[i]);
            }
            else
            {
                sprite.printf("  %s", menuItems[i]);
            }
        }
        sprite.pushSprite(0, 0); // Push the sprite to the screen
        m5s_button_update();

        if (m5s_button_is_pressed(BUTTON_A))
        {
            switch (menuIndex)
            {
            case 0:
                sprite.fillScreen(BLACK);
                sprite.setCursor(0, 0);
                sprite.printf("Battery Level %ld %%", M5.Power.getBatteryLevel());
                sprite.pushSprite(0, 0);
                vTaskDelay(2000 / portTICK_PERIOD_MS); // Delay to show the battery level
                break;
            case 1:
                while (!m5s_button_is_pressed(BUTTON_C)) // Exit button test with Button C
                {
                    sprite.fillScreen(BLACK);
                    sprite.setCursor(0, 0);
                    sprite.printf("Button A - %3s - %4ld s", m5s_button_is_pressed(BUTTON_A) ? "ON" : "OFF", m5s_button_get_timer_s(BUTTON_A));
                    sprite.setCursor(0, 20);
                    sprite.printf("Button B - %3s - %4ld s", m5s_button_is_pressed(BUTTON_B) ? "ON" : "OFF", m5s_button_get_timer_s(BUTTON_B));
                    sprite.setCursor(0, 40);
                    m5s_button_update();
                    sprite.pushSprite(0, 0);
                    vTaskDelay(10 / portTICK_PERIOD_MS); // Update rate
                }
                break;
            case 2:
                return; // Exit the app
            }
        }
        else if (m5s_button_is_pressed(BUTTON_B))
        {
            menuIndex = (menuIndex + 1) % menuSize; // Move to the next menu item
            vTaskDelay(200 / portTICK_PERIOD_MS);   // Debounce delay
        }
        vTaskDelay(10 / portTICK_PERIOD_MS); // Main loop delay
    }
}
