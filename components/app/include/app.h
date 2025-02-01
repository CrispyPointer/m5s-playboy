#ifndef APP_H
#define APP_H

typedef enum
{
    UNKNOWN_PAGE = 0,
    MENU_PAGE = 1,
    NEW_GAME_PAGE = 2,
    DEVICE_PAGE = 3,
    SETTINGS_PAGE = 4,
} PAGE_T;

typedef enum
{
    NEW_GAME = 0,
    DEVICE,
    SETTINGS,
    EXIT,
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