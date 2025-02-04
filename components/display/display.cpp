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

#ifdef USE_SPRITE
LGFX_Sprite sprite(&M5.Lcd);
#endif

static void display_clear(void)
{
#ifdef USE_SPRITE
    sprite.fillScreen(BLACK); // Clear the sprite
#else
    M5.Lcd.fillScreen(BLACK);
#endif
}

static void display_update(void)
{
#ifdef USE_SPRITE
    sprite.pushSprite(0, 0); // Push the sprite to the screen
#endif
}

static void display_set_cursor(int x, int y)
{
#ifdef USE_SPRITE

    sprite.setCursor(x, y); // Set the cursor position
#else
    M5.Lcd.setCursor(x, y); // Set the cursor position
#endif
}

static void display_print(const char* string)
{
#ifdef USE_SPRITE
    sprite.printf("%s", string); // Print text to the sprite
#else
    M5.Lcd.printf("%s", string); // Print text to the sprite
#endif
}

static void display_progress_bar(int x, int y, int w, int h, uint8_t val, uint16_t color)
{
#ifdef USE_SPRITE
    sprite.drawRect(x, y, w, h, color);                                       // Draw the progress bar outline
    sprite.fillRect(x + 1, y + 1, w * (((float)val) / 100.0f), h - 1, color); // Fill the progress bar
#else
    (void)color;
    M5.Lcd.drawRect(x, y, w, h, color);                                       // Draw the progress bar outline
    M5.Lcd.fillRect(x + 1, y + 1, w * (((float)val) / 100.0f), h - 1, color); // Fill the progress bar
#endif
}

static void display_load_intro(void)
{
#ifdef USE_SPRITE
    sprite.setTextSize(4);
    sprite.setTextFont(2); // Assuming font 2 is a nice font, adjust as needed
#else
    M5.Lcd.setTextSize(4);
    M5.Lcd.setTextFont(2); // Assuming font 2 is a nice font, adjust as needed
#endif
    display_set_cursor(60, 80); // Adjust the position to center the text
    display_print("NEKOCO");

    for (uint32_t i = 0; i < 100; i += 10)
    {
        display_progress_bar(40, 180, 240, 40, i, WHITE); // Draw a progress bar
        display_update();                                 // Update the display
        vTaskDelay(100 / portTICK_PERIOD_MS);             // Delay for 10ms
    }
}

static void display_setup_menu_frame(void)
{
#ifdef USE_SPRITE
    /** Draw instruction at the bottom of the screen to navigate the menu*/
    sprite.fillTriangle(55, 210, 75, 210, 65, 230, WHITE);    // Draw down button
    sprite.fillTriangle(145, 230, 165, 230, 155, 210, WHITE); // Draw up button

    // Draw enter button
    sprite.fillRect(240, 215, 20, 5, WHITE);                  // button body (rectangle)
    sprite.fillTriangle(260, 210, 260, 225, 270, 217, WHITE); // button head (triangle)
#else
    /** Draw instruction at the bottom of the screen to navigate the menu*/
    M5.Lcd.fillTriangle(55, 210, 75, 210, 65, 230, WHITE);    // Draw down button
    M5.Lcd.fillTriangle(145, 230, 165, 230, 155, 210, WHITE); // Draw up button

    // Draw enter button
    M5.Lcd.fillRect(240, 215, 20, 5, WHITE);                  // button body (rectangle)
    M5.Lcd.fillTriangle(260, 210, 260, 225, 270, 217, WHITE); // button head (triangle)
#endif
}

static void display_setup_device_frame(uint32_t soc)
{
#ifdef USE_SPRITE
    /** Draw battery frame -- horizontally */
    sprite.drawRect(15, 80, 200, 100, WHITE);                 // Draw battery main body
    sprite.fillRect(215, 105, 25, 50, WHITE);                 // Draw battery tip
    sprite.fillRect(20, 85, 190 * (soc / 100.0f), 90, GREEN); // Draw battery level

    // Draw exit button
    sprite.fillRect(55, 215, 20, 5, WHITE);                // button body (rectangle)
    sprite.fillTriangle(55, 210, 55, 225, 45, 217, WHITE); // button head (triangle)
#else
    /** Draw battery frame -- horizontally */
    M5.Lcd.drawRect(15, 80, 200, 100, WHITE);                 // Draw battery main body
    M5.Lcd.fillRect(215, 105, 25, 50, WHITE);                 // Draw battery tip
    M5.Lcd.fillRect(20, 85, 190 * (soc / 100.0f), 90, GREEN); // Draw battery level

    // Draw exit button
    M5.Lcd.fillRect(55, 215, 20, 5, WHITE);                // button body (rectangle)
    M5.Lcd.fillTriangle(55, 210, 55, 225, 45, 217, WHITE); // button head (triangle)
#endif
}

extern "C" void display_init(void)
{
    M5.Lcd.setBrightness(50); // BRIGHTNESS = MAX 255
                              //sprite.createSprite(320, 240); // Create a sprite for the display
#ifdef USE_SPRITE
    if (sprite.createSprite(320, 240) == nullptr)
    {
        printf("Error: sprite not created, not enough free RAM!");
        M5.Lcd.printf("Error: sprite not created, not enough free RAM!");
    }
    else
    {
        printf("Yahoo, success, sprite created!");
    }
#endif

    // Load the intro page
    display_clear();
    // display_load_intro();

    // Set the text size and color for normal functions
#ifdef USE_SPRITE
    sprite.setTextSize(2);      // Set text size to 2
    sprite.setTextColor(WHITE); // Set text color to white
    sprite.setTextFont(0);
#else
    M5.Lcd.setTextSize(2);      // Set text size to 2
    M5.Lcd.setTextColor(WHITE); // Set text color to white
    M5.Lcd.setTextFont(0);
#endif
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
#ifdef USE_SPRITE
    sprite.setTextSize(3);
#else
    M5.Lcd.setTextSize(3);
#endif
    display_print(std::to_string(soc).c_str());
    display_print("%");
#ifdef USE_SPRITE
    sprite.setTextSize(2);
#else
    M5.Lcd.setTextSize(2);
#endif
    display_update(); // Update the display with the new device information
}

extern "C" void display_tetris_clear(void)
{
    display_clear();
}

extern "C" void display_tetris_set_bg(const uint8_t* data)
{
    (void)data;
    // For now just clear the screen
    display_clear();
}

extern "C" void display_tetris_draw(const uint8_t* data)
{
    if (data == NULL)
    {
        printf("Error: data is null\n");
        return;
    }

    // display_update(); // Update the display with the new device information
    // display_clear();
    M5.Lcd.drawBitmap(100, 0, 120, 240, data);
}
