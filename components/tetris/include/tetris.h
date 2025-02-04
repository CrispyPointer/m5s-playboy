#ifndef TETRIS_H
#define TETRIS_H

#ifdef __cplusplus
extern "C"
{
#endif

    void tetris_app_init(void);

    void tetris_start(void);

    bool tetris_is_ended(void);

#ifdef __cplusplus
}
#endif

#endif // TETRIS_H