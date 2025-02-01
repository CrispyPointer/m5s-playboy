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

static void display_clear(void)
{
    sprite.fillScreen(BLACK); // Clear the sprite
}

static void display_update(void)
{
    sprite.pushSprite(0, 0); // Push the sprite to the screen
}

static void display_set_cursor(int x, int y)
{
    sprite.setCursor(x, y); // Set the cursor position
}

static void display_print(const char* string)
{
    sprite.printf("%s", string); // Print text to the sprite
}

static void display_progress_bar(int x, int y, int w, int h, uint8_t val, uint16_t color)
{
    sprite.drawRect(x, y, w, h, color);                                       // Draw the progress bar outline
    sprite.fillRect(x + 1, y + 1, w * (((float)val) / 100.0f), h - 1, color); // Fill the progress bar
}

static void display_load_intro(void)
{
    display_clear(); // Clear the display

    sprite.setTextSize(4);
    sprite.setTextFont(2);       // Assuming font 2 is a nice font, adjust as needed
    display_set_cursor(60, 80); // Adjust the position to center the text
    display_print("NEKOCO");

    for (uint32_t i = 0; i < 100; i += 5)
    {
        display_progress_bar(40, 180, 240, 40, i, WHITE); // Draw a progress bar
        display_update();                                 // Update the display
        vTaskDelay(100 / portTICK_PERIOD_MS);             // Delay for 10ms
    }
}

static void display_setup_menu_frame(void)
{
    /** Draw instruction at the bottom of the screen to navigate the menu*/
    sprite.fillTriangle(55, 210, 75, 210, 65, 230, WHITE);    // Draw down button
    sprite.fillTriangle(145, 230, 165, 230, 155, 210, WHITE); // Draw up button

    // Draw enter button
    sprite.fillRect(240, 215, 20, 5, WHITE);                  // button body (rectangle)
    sprite.fillTriangle(260, 210, 260, 225, 270, 217, WHITE); // button head (triangle)
}

static void display_setup_device_frame(uint32_t soc)
{
    /** Draw battery frame -- horizontally */
    sprite.drawRect(15, 80, 200, 100, WHITE);              // Draw battery main body
    sprite.fillRect(215, 105, 25, 50, WHITE);              // Draw battery tip
    sprite.fillRect(20, 85, 190 * (soc / 100.0f), 90, GREEN); // Draw battery level

    // Draw exit button
    sprite.fillRect(55, 215, 20, 5, WHITE);                // button body (rectangle)
    sprite.fillTriangle(55, 210, 55, 225, 45, 217, WHITE); // button head (triangle)
}

extern "C" void display_init(void)
{
    M5.Lcd.setBrightness(50);      // BRIGHTNESS = MAX 255
    sprite.createSprite(320, 240); // Create a sprite for the display

    // Load the intro page
    display_load_intro();

    // Set the text size and color for normal functions
    sprite.setTextSize(2);      // Set text size to 2
    sprite.setTextColor(WHITE); // Set text color to white
    sprite.setTextFont(0);
    display_clear();
    display_update();
}

extern "C" void display_draw_menu(const char* menu_items[], int menu_size, int menu_index)
{
    display_clear();

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

extern "C" void display_draw_device(uint32_t soc, uint32_t volt, uint32_t curr)
{
    (void)volt; // Not supported for M5S
    (void)curr; // Not supported for M5S
    display_clear();

    display_setup_device_frame(soc);
    display_set_cursor(15, 0);
    display_print("Device Information");
    display_set_cursor(15, 40);
    sprite.setTextSize(3);
    display_print(std::to_string(soc).c_str());
    display_print("%");
    sprite.setTextSize(2);

    display_update(); // Update the display with the new device information
}