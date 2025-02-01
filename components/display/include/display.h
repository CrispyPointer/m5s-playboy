#ifndef DISAPLAY_H
#define DISAPLAY_H

#ifdef __cplusplus
extern "C" {
#endif
void display_draw_menu(const char* menu_items[], int menu_size, int menu_index);

void display_print(const char* string);

void display_set_cursor(int x, int y);

void display_update(void);

void display_clear(void);

void display_init(void);

#ifdef __cplusplus
}
#endif

#endif // DISAPLAY_H
