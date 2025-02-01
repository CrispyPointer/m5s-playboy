#ifndef APP_H
#define APP_H

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

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

void app_task_init(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // APP_H