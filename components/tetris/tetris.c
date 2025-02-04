//========================================================================
// TETRIS with M5STACK : 2018.01.20 Transplant by macsbug
// Controller : Buttons A = LEFT, B = RIGHT, C = START, ROTATE
// Display    : Left = 100x240, Center = 120x240, Right = 100x240
// Block      : 8ea, 12x12 pixel
// SD         : tetris.jpg : BackGround Image : R.G.B 320x240 pixel
// Github     : https://macsbug.wordpress.com/2018/01/20/tetris-with-m5stack/
//========================================================================
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

#include "tetris.h"
#include "display.h"
#include "defines.h"
#include "tiny_timer.h"
#include "m5s_button.h"

#include "esp_system.h"
#include "esp_log.h"

#define LENGTH 12 // the number of pixels for a side of a block
#define WIDTH  10 // the number of horizontal blocks
#define HEIGHT 20 // the number of vertical blocks

static bool start = false;

static uint16_t BlockImage[8][12][12];  // Block
static uint16_t backBuffer[240][120];   // GAME AREA
static int screen[WIDTH][HEIGHT] = {0}; // it shows color-numbers of all positions
typedef struct
{
    int X, Y;
} Point;
typedef struct Block
{
    Point square[4][4];
    uint32_t numRotate, color;
} Block;

Point pos;
Block block;

static int rot, fall_cnt = 0;
static bool started = false, gameover = false;
static bool btn_mid = false, btn_left = false, btn_right = false;
static int game_speed = 50; // 50msec
static Block blocks[7] = {{{{{-1, 0}, {0, 0}, {1, 0}, {2, 0}}, {{0, -1}, {0, 0}, {0, 1}, {0, 2}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}}, 2, 1},
                          {{{{0, -1}, {1, -1}, {0, 0}, {1, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}}, 1, 2},
                          {{{{-1, -1}, {-1, 0}, {0, 0}, {1, 0}}, {{-1, 1}, {0, 1}, {0, 0}, {0, -1}}, {{-1, 0}, {0, 0}, {1, 0}, {1, 1}}, {{1, -1}, {0, -1}, {0, 0}, {0, 1}}}, 4, 3},
                          {{{{-1, 0}, {0, 0}, {0, 1}, {1, 1}}, {{0, -1}, {0, 0}, {-1, 0}, {-1, 1}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}}, 2, 4},
                          {{{{-1, 0}, {0, 0}, {1, 0}, {1, -1}}, {{-1, -1}, {0, -1}, {0, 0}, {0, 1}}, {{-1, 1}, {-1, 0}, {0, 0}, {1, 0}}, {{0, -1}, {0, 0}, {0, 1}, {1, 1}}}, 4, 5},
                          {{{{-1, 1}, {0, 1}, {0, 0}, {1, 0}}, {{0, -1}, {0, 0}, {1, 0}, {1, 1}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}}, 2, 6},
                          {{{{-1, 0}, {0, 0}, {1, 0}, {0, -1}}, {{0, -1}, {0, 0}, {0, 1}, {-1, 0}}, {{-1, 0}, {0, 0}, {1, 0}, {0, 1}}, {{0, -1}, {0, 0}, {0, 1}, {1, 0}}}, 4, 7}};

//========================================================================

static void ClearKeys(void)
{
    btn_mid = false;
    btn_left = false;
    btn_right = false;
}

static bool is_esc_hit(void)
{
    if (m5s_button_is_pressed(2u) && (m5s_button_get_timer_s(2u) > 5u))
    {
        return true;
    }
    else
    {
        return false;
    }
}

//========================================================================
static bool KeyPadLoop(void)
{
    static bool left_cache, right_cache, mid_cache;
    if (m5s_button_is_pressed(0u) && !left_cache)
    {
        ClearKeys();
        btn_left = true;
        left_cache = true;
        return true;
    }
    else if (m5s_button_was_pressed(0u))
    {
        left_cache = false;
    }

    if (m5s_button_is_pressed(1u) && !mid_cache)
    {
        ClearKeys();
        btn_mid = true;
        mid_cache = true;
        return true;
    }
    else if (m5s_button_was_pressed(1u))
    {
        mid_cache = false;
    }

    if (m5s_button_is_pressed(2u) && !right_cache)
    {
        ClearKeys();
        btn_right = true;
        right_cache = true;
        return true;
    }
    else if (m5s_button_was_pressed(2u))
    {
        right_cache = false;
    }

    return false;
}

//========================================================================
static bool GetSquares(Block block, Point pos, int rot, Point* squares)
{
    bool overlap = false;
    for (int i = 0; i < 4; ++i)
    {
        Point p;
        p.X = pos.X + block.square[rot][i].X;
        p.Y = pos.Y + block.square[rot][i].Y;
        overlap |= p.X < 0 || p.X >= WIDTH || p.Y < 0 || p.Y >= HEIGHT || screen[p.X][p.Y] != 0;
        squares[i] = p;
    }
    return !overlap;
}
//========================================================================

static void GameOver(void)
{
    for (int i = 0; i < WIDTH; ++i)
    {
        for (int j = 0; j < HEIGHT; ++j)
        {
            if (screen[i][j] != 0)
            {
                screen[i][j] = 4;
            }
        }
    }
    gameover = true;
}

static void DeleteLine(void)
{
    for (int j = 0; j < HEIGHT; ++j)
    {
        bool Delete = true;
        for (int i = 0; i < WIDTH; ++i)
        {
            if (screen[i][j] == 0)
            {
                Delete = false;
            }
        }
        if (Delete)
        {
            for (int k = j; k >= 1; --k)
            {
                for (int i = 0; i < WIDTH; ++i)
                {
                    screen[i][k] = screen[i][k - 1];
                }
            }
        }
    }
}

static uint32_t get_random(uint32_t min, uint32_t max)
{
    uint32_t ret;

    ret = rand() % (max - min + 1) + min;

    return ret;
}

static void PutStartPos(void)
{
    pos.X = 4;
    pos.Y = 1;
    block = blocks[get_random(1u, 6u)];
    rot = get_random(0u, block.numRotate);
}

//========================================================================

//========================================================================
static void Draw(void)
{ // Draw 120x240 in the center
    for (int i = 0; i < WIDTH; ++i)
    {
        for (int j = 0; j < HEIGHT; ++j)
        {
            for (int k = 0; k < LENGTH; ++k)
            {
                for (int l = 0; l < LENGTH; ++l)
                {
                    backBuffer[j * LENGTH + l][i * LENGTH + k] = BlockImage[screen[i][j]][k][l];
                }
            }
        }
    }
    display_tetris_draw((uint8_t*)backBuffer);
}
//========================================================================

//========================================================================
static void ReviseScreen(Point next_pos, int next_rot)
{
    if (!started)
    {
        return;
    }
    Point next_squares[4];
    for (int i = 0; i < 4; ++i)
    {
        screen[pos.X + block.square[rot][i].X][pos.Y + block.square[rot][i].Y] = 0;
    }
    if (GetSquares(block, next_pos, next_rot, next_squares))
    {
        for (int i = 0; i < 4; ++i)
        {
            screen[next_squares[i].X][next_squares[i].Y] = block.color;
        }
        pos = next_pos;
        rot = next_rot;
    }
    else
    {
        for (int i = 0; i < 4; ++i)
        {
            screen[pos.X + block.square[rot][i].X][pos.Y + block.square[rot][i].Y] = block.color;
        }
        if (next_pos.Y == pos.Y + 1)
        {
            DeleteLine();
            PutStartPos();
            if (!GetSquares(block, pos, rot, next_squares))
            {
                for (int i = 0; i < 4; ++i)
                {
                    screen[pos.X + block.square[rot][i].X][pos.Y + block.square[rot][i].Y] = block.color;
                }
                GameOver();
            }
        }
    }
    Draw();
}

//========================================================================
void make_block(int n, uint16_t color)
{ // Make Block color
    for (int i = 0; i < 12; i++)
    {
        for (int j = 0; j < 12; j++)
        {
            BlockImage[n][i][j] = color; // Block color
            if (i == 0 || j == 0)
            {
                BlockImage[n][i][j] = 0; // BLACK Line
            }
        }
    }
}
//========================================================================

static void tetris_init(void)
{
    //----------------------------// Make bBlock ----------------------------
    make_block(0, 0x0000); // Type No, Color
    make_block(1, 0xF00F); // _DD,DD_  LIGHT GREEN
    make_block(2, 0xFF87); // DD_,_DD  GREEN
    make_block(3, 0x00F0); // DDDD      RED
    make_block(4, 0xFBE4); // DD,DD    PUPLE
    make_block(5, 0xFF00); // D__,DDD  BLUE
    make_block(6, 0x87FF); // __D,DDD  YELLO
    make_block(7, 0xF8FC); // _D_,DDD  PINK

    //----------------------------------------------------------------------
    //M5.Lcd.drawJpgFile(SD, "/tetris.jpg");     // @todo Load background from SD
    //@todo Load background from file data
    //display_tetris_set_bg(tetris_img);
    PutStartPos(); // Start Position
    for (int i = 0; i < 4; ++i)
    {
        screen[pos.X + block.square[rot][i].X][pos.Y + block.square[rot][i].Y] = block.color;
    }
}

//========================================================================
void GetNextPosRot(Point* pnext_pos, int* pnext_rot)
{
    bool received = KeyPadLoop();
    if (btn_mid)
    {
        started = true;
    }
    if (!started)
    {
        return;
    }
    pnext_pos->X = pos.X;
    pnext_pos->Y = pos.Y;
    if ((fall_cnt = (fall_cnt + 1) % 10) == 0)
    {
        pnext_pos->Y += 1;
    }
    else if (received)
    {
        if (btn_left)
        {
            btn_left = false;
            pnext_pos->X -= 1;
        }
        else if (btn_right)
        {
            btn_right = false;
            pnext_pos->X += 1;
        }
        else if (btn_mid)
        {
            btn_mid = false;
            *pnext_rot = (*pnext_rot + block.numRotate - 1) % block.numRotate;
        }
    }
}
//========================================================================

//========================================================================
static void tetris_proc(void)
{
    while (true)
    {
        static bool entered;
        bool is_esc = is_esc_hit();
        if (start && !is_esc)
        {
            if (entered)
            {
                if (gameover)
                {
                    is_esc = true;
                }
                else
                {
                    Point next_pos;
                    int next_rot = rot;
                    GetNextPosRot(&next_pos, &next_rot);
                    ReviseScreen(next_pos, next_rot);
                }
            }
            else
            {
                entered = true;
            }
        }
        else
        {
            start = false;
        }
        timer_delay_ms(game_speed); // SPEED ADJUST
    }
}

bool tetris_is_ended(void)
{
    if(!start)
    {
        display_tetris_clear();
        return true;
    }
    else
    {
        return false;
    }
}

void tetris_start(void)
{
    start = true;
    display_tetris_set_bg(NULL);
    Draw();
}

void tetris_app_init(void)
{
    tetris_init();
    xTaskCreate((TaskFunction_t)tetris_proc, "tetris_proc", 4096, NULL, 4, NULL);
}