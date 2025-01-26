#ifndef APP_H
#define APP_H

#define MENU_ITEM_NUM 5

typedef enum
{
    UNKNOWN_PAGE = 0,
    MAIN_MENU = 1,
    NEW_GAME_MENU = 2,
    LOAD_GAME_MENU = 3,
    SETTINGS_MENU = 4,
} PAGE_T;

typedef enum
{
    UNKNOWN = 0,
    NEW_GAME = 1,
    LOAD_GAME = 2,
    SETTINGS = 3,
    EXIT = 4,
} MENU_ITEM_T;

typedef struct
{
    PAGE_T page;
    uint8_t menu_index;
    const char *menu_items[5];
} APP_DATA_T;

#endif // APP_H