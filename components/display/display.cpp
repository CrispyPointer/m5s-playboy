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

#include "display.h"
#include "defines.h"
#include "M5Unified.h"

LGFX_Sprite sprite(&M5.Lcd);

extern "C" void display_init(void)
{
    M5.Lcd.setBrightness(50);      // BRIGHTNESS = MAX 255
    sprite.createSprite(320, 240); // Create a sprite for the display
    sprite.setTextSize(2);         // Set text size to 2
    sprite.setTextColor(WHITE);    // Set text color to white
    display_clear();
    display_update();
}

extern "C" void display_clear(void)
{
    sprite.fillScreen(BLACK); // Clear the sprite
}

extern "C" void display_update(void)
{
    sprite.pushSprite(0, 0); // Push the sprite to the screen
}

extern "C" void display_set_cursor(int x, int y)
{
    sprite.setCursor(x, y); // Set the cursor position
}

extern "C" void display_print(const char* string)
{
    sprite.printf("%s", string); // Print text to the sprite
}

static void display_setup_menu_frame(void)
{
    display_clear(); // Clear the display

    /** Draw instruction at the bottom of the screen to navigate the menu*/
    sprite.fillTriangle(55, 210, 75, 210, 65, 230, WHITE);    // Draw down button
    sprite.fillTriangle(145, 230, 165, 230, 155, 210, WHITE); // Draw up button

    // Draw enter button
    sprite.fillRect(240, 215, 20, 5, WHITE);              // button body (rectangle)
    sprite.fillTriangle(260, 210, 260, 225, 270, 217, WHITE); // button head (triangle)
}

extern "C" void display_draw_menu(const char* menu_items[], int menu_size, int menu_index)
{
    display_setup_menu_frame();
    for (int i = 0; i < menu_size; ++i)
    {
        display_set_cursor(15, 40 * i);
        if (i == menu_index)
        {
            display_print("> ");
            display_print(menu_items[i]);
        }
        else
        {
            display_print(menu_items[i]);
        }
    }
    display_update(); // Update the display with the new menu
}
