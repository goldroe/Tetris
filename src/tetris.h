#ifndef TETRIS_H
#define TETRIS_H

#include <stdint.h>
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t  s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef float  f32;
typedef double f64;

#define for_array(IT, T, ARR) \
    for (T IT = 0; (IT) < sizeof((ARR)) / sizeof((ARR)[0]); IT++)

enum {
    Yellow,
    Red,
    Green,
    Purple,
    Orange,
    Cyan,
    Blue
};

struct Cell {
    int x;
    int y;
    bool available;
    int color;
};

struct Board {
    Cell *cells;
    int rows;
    int cols;
};

enum TetradKind {
    Tetrad_Invalid,
    Tetrad_I,
    Tetrad_O,
    Tetrad_J,
    Tetrad_L,
    Tetrad_S,
    Tetrad_Z,
    Tetrad_T,

    Tetrad_Count
};

enum {
    LEFT,
    UP,
    RIGHT,
    DOWN
};

struct Tetrad {
    TetradKind kind;
    int x;
    int y;
    Cell cells[4];
    int dir;
};

enum GameMode {
    GameMode_Menu,
    GameMode_Playing,
    GameMode_Paused,
    GameMode_GameOver,
    GameMode_Quit,
};

struct GameState {
    GameMode mode;

    int player_score;
    Board board;
    Tetrad tetrad;
    Tetrad next_tetrad;
    Tetrad held_tetrad;
    bool tetrad_swapped;

    float move_timer;
    float move_t;
};

#endif // TETRIS_H
