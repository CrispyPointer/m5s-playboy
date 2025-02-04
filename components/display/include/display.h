#ifndef DISAPLAY_H
#define DISAPLAY_H

#ifdef __cplusplus
extern "C"
{
#endif

    void display_draw_menu(const char* menu_items[], int menu_size, int menu_index);

    void display_draw_device(uint32_t soc, uint32_t volt, uint32_t curr);

    void display_tetris_set_bg(const uint8_t* data);

    void display_tetris_draw(const uint8_t* data);
    
    void display_tetris_clear(void);

    void display_init(void);

#ifdef __cplusplus
}
#endif

#endif // DISAPLAY_H
