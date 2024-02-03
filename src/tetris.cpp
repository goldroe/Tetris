// TODO: Multiplayer client-server
// TODO: Particles and lighting
// TODO: Textures
// TODO: Clean up rotation (split into offsets corresponding to tetrad)
// TODO: Better responding to rotation on bounds (try to rotate by shifting if necessary)

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SUPPORT_RPRAND_GENERATOR
#include <raylib.h>
#include <raymath.h>

#include "tetris.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

GameMode game_mode;

Texture2D tile_texture;

Board make_board(int rows, int cols) {
    Board board;
    board.rows = rows;
    board.cols = cols;
    board.cells = (Cell *)malloc(board.rows * board.cols * sizeof(Cell));
    for (int y = 0; y < board.rows; y++) {
        for (int x = 0; x < board.cols; x++) {
            int i = y * board.cols + x;
            Cell *cell = &board.cells[i];
            cell->x = x;
            cell->y = y;
            cell->available = true;
        }
    }
    return board;
}

Cell *cell_at(Board board, int x, int y) {
    Cell *cell = nullptr;
    if (x >= 0 && x < board.cols &&
        y >= 0 && y < board.rows) {
        cell = &board.cells[y * board.cols + x];
    }
    return cell;
}

Cell *request_cell(Board board, int x, int y) {
    if (x >= 0 && x < board.cols &&
        y >= 0 && y < board.rows) {
        Cell *cell = &board.cells[y * board.cols + x];
        if (cell->available) return cell;
    }
    return nullptr;
}

bool rotate_tetrad_L(Tetrad *tetrad, Board board) {
    Cell cells[4] = {};
    switch (tetrad->dir) {
    case LEFT:
        cells[0] = {tetrad->x + 1, tetrad->y + 0};
        cells[1] = {tetrad->x + 1, tetrad->y + 1};
        cells[2] = {tetrad->x + 1, tetrad->y + 2};
        cells[3] = {tetrad->x + 2, tetrad->y + 2};
        break; 
    case UP:
        cells[0] = {tetrad->x + 0, tetrad->y + 2};
        cells[1] = {tetrad->x + 0, tetrad->y + 1};
        cells[2] = {tetrad->x + 1, tetrad->y + 1};
        cells[3] = {tetrad->x + 2, tetrad->y + 1};
        break; 
    case RIGHT:
        cells[0] = {tetrad->x + 0, tetrad->y + 0};
        cells[1] = {tetrad->x + 1, tetrad->y + 0};
        cells[2] = {tetrad->x + 1, tetrad->y + 1};
        cells[3] = {tetrad->x + 1, tetrad->y + 2};
        break; 
    case DOWN:
        cells[0] = {tetrad->x + 0, tetrad->y + 1};
        cells[1] = {tetrad->x + 1, tetrad->y + 1};
        cells[2] = {tetrad->x + 2, tetrad->y + 1};
        cells[3] = {tetrad->x + 2, tetrad->y + 0};
        break; 
    }
    for_array(i, int, cells) {
        if (!request_cell(board, cells[i].x, cells[i].y)) {
            return false;
        }
    }
    for_array(i, int, cells) {
        tetrad->cells[i].x = cells[i].x;
        tetrad->cells[i].y = cells[i].y;
    }
    return true;
}

bool rotate_tetrad_I(Tetrad *tetrad, Board board) {
    Cell cells[4] = {};

    switch (tetrad->dir) {
    case LEFT:
        cells[0] = {tetrad->x + 2, tetrad->y + 0};
        cells[1] = {tetrad->x + 2, tetrad->y + 1};
        cells[2] = {tetrad->x + 2, tetrad->y + 2};
        cells[3] = {tetrad->x + 2, tetrad->y + 3};
        break;
    case UP:
        cells[0] = {tetrad->x + 0, tetrad->y + 2};
        cells[1] = {tetrad->x + 1, tetrad->y + 2};
        cells[2] = {tetrad->x + 2, tetrad->y + 2};
        cells[3] = {tetrad->x + 3, tetrad->y + 2};
        break;
    case RIGHT:
        cells[0] = {tetrad->x + 1, tetrad->y + 0};
        cells[1] = {tetrad->x + 1, tetrad->y + 1};
        cells[2] = {tetrad->x + 1, tetrad->y + 2};
        cells[3] = {tetrad->x + 1, tetrad->y + 3};
        break;
    case DOWN:
        cells[0] = {tetrad->x + 0, tetrad->y + 1};
        cells[1] = {tetrad->x + 1, tetrad->y + 1};
        cells[2] = {tetrad->x + 2, tetrad->y + 1};
        cells[3] = {tetrad->x + 3, tetrad->y + 1};
        break;
    }

    for_array(i, int, cells) {
        if (!request_cell(board, cells[i].x, cells[i].y)) {
            return false;
        }
    }
    for_array(i, int, cells) {
        tetrad->cells[i].x = cells[i].x;
        tetrad->cells[i].y = cells[i].y;
    }
    return true; 
}

bool rotate_tetrad_J(Tetrad *tetrad, Board board) {
    Cell cells[4] = {};
    switch (tetrad->dir) {
    case LEFT:
        cells[0] = {tetrad->x + 2, tetrad->y + 0};
        cells[1] = {tetrad->x + 1, tetrad->y + 0};
        cells[2] = {tetrad->x + 1, tetrad->y + 1};
        cells[3] = {tetrad->x + 1, tetrad->y + 2};
        break;
    case UP:
        cells[0] = {tetrad->x + 0, tetrad->y + 1};
        cells[1] = {tetrad->x + 1, tetrad->y + 1};
        cells[2] = {tetrad->x + 2, tetrad->y + 1};
        cells[3] = {tetrad->x + 2, tetrad->y + 2};
        break;
    case RIGHT:
        cells[0] = {tetrad->x + 0, tetrad->y + 2};
        cells[1] = {tetrad->x + 1, tetrad->y + 2};
        cells[2] = {tetrad->x + 1, tetrad->y + 1};
        cells[3] = {tetrad->x + 1, tetrad->y + 0};
        break;
    case DOWN:
        cells[0] = {tetrad->x + 0, tetrad->y + 0};
        cells[1] = {tetrad->x + 0, tetrad->y + 1};
        cells[2] = {tetrad->x + 1, tetrad->y + 1};
        cells[3] = {tetrad->x + 2, tetrad->y + 1};
        break;
    }

    for_array(i, int, cells) {
        if (!request_cell(board, cells[i].x, cells[i].y)) {
            return false;
        }
    }
    for_array(i, int, cells) {
        tetrad->cells[i].x = cells[i].x;
        tetrad->cells[i].y = cells[i].y;
    }
    return true; 
}

bool rotate_tetrad_T(Tetrad *tetrad, Board board) {
    Cell cells[4] = {};
    switch (tetrad->dir) {
    case LEFT:
        cells[0] = {tetrad->x + 1, tetrad->y + 0};
        cells[1] = {tetrad->x + 1, tetrad->y + 1};
        cells[2] = {tetrad->x + 1, tetrad->y + 2};
        cells[3] = {tetrad->x + 2, tetrad->y + 1};
        break;
    case UP:
        cells[0] = {tetrad->x + 0, tetrad->y + 1};
        cells[1] = {tetrad->x + 1, tetrad->y + 1};
        cells[2] = {tetrad->x + 2, tetrad->y + 1};
        cells[3] = {tetrad->x + 1, tetrad->y + 2};
        break;
    case RIGHT:
        cells[0] = {tetrad->x + 0, tetrad->y + 1};
        cells[1] = {tetrad->x + 1, tetrad->y + 0};
        cells[2] = {tetrad->x + 1, tetrad->y + 1};
        cells[3] = {tetrad->x + 1, tetrad->y + 2};
        break;
    case DOWN:
        cells[0] = {tetrad->x + 0, tetrad->y + 1};
        cells[1] = {tetrad->x + 1, tetrad->y + 1};
        cells[2] = {tetrad->x + 2, tetrad->y + 1};
        cells[3] = {tetrad->x + 1, tetrad->y + 0};
        break;
    }

    for_array(i, int, cells) {
        if (!request_cell(board, cells[i].x, cells[i].y)) {
            return false;
        }
    }
    for_array(i, int, cells) {
        tetrad->cells[i].x = cells[i].x;
        tetrad->cells[i].y = cells[i].y;
    }
    return true; 
}

bool rotate_tetrad_S(Tetrad *tetrad, Board board) {
    Cell cells[4] = {};
    switch (tetrad->dir) {
    case LEFT:
        cells[0] = {tetrad->x + 1, tetrad->y + 0};
        cells[1] = {tetrad->x + 1, tetrad->y + 1};
        cells[2] = {tetrad->x + 2, tetrad->y + 1};
        cells[3] = {tetrad->x + 2, tetrad->y + 2};
        break;
    case UP:
        cells[0] = {tetrad->x + 0, tetrad->y + 1};
        cells[1] = {tetrad->x + 1, tetrad->y + 1};
        cells[2] = {tetrad->x + 1, tetrad->y + 0};
        cells[3] = {tetrad->x + 2, tetrad->y + 0};
        break;
    case RIGHT:
        cells[0] = {tetrad->x + 0, tetrad->y + 0};
        cells[1] = {tetrad->x + 0, tetrad->y + 1};
        cells[2] = {tetrad->x + 1, tetrad->y + 1};
        cells[3] = {tetrad->x + 1, tetrad->y + 2};
        break;
    case DOWN:
        cells[0] = {tetrad->x + 0, tetrad->y + 1};
        cells[1] = {tetrad->x + 1, tetrad->y + 1};
        cells[2] = {tetrad->x + 1, tetrad->y + 0};
        cells[3] = {tetrad->x + 2, tetrad->y + 0};
        break;
    }

    for_array(i, int, cells) {
        if (!request_cell(board, cells[i].x, cells[i].y)) {
            return false;
        }
    }
    for_array(i, int, cells) {
        tetrad->cells[i].x = cells[i].x;
        tetrad->cells[i].y = cells[i].y;
    }
    return true; 
}

bool rotate_tetrad_Z(Tetrad *tetrad, Board board) {
    Cell cells[4] = {};
    switch (tetrad->dir) {
    case LEFT:
        cells[0] = {tetrad->x + 1, tetrad->y + 1};
        cells[1] = {tetrad->x + 1, tetrad->y + 2};
        cells[2] = {tetrad->x + 2, tetrad->y + 1};
        cells[3] = {tetrad->x + 2, tetrad->y + 0};
        break;
    case UP:
        cells[0] = {tetrad->x + 0, tetrad->y + 1};
        cells[1] = {tetrad->x + 1, tetrad->y + 1};
        cells[2] = {tetrad->x + 1, tetrad->y + 2};
        cells[3] = {tetrad->x + 2, tetrad->y + 2};
        break;
    case RIGHT:
        cells[0] = {tetrad->x + 0, tetrad->y + 2};
        cells[1] = {tetrad->x + 0, tetrad->y + 1};
        cells[2] = {tetrad->x + 1, tetrad->y + 1};
        cells[3] = {tetrad->x + 1, tetrad->y + 0};
        break;
    case DOWN:
        cells[0] = {tetrad->x + 0, tetrad->y + 0};
        cells[1] = {tetrad->x + 1, tetrad->y + 0};
        cells[2] = {tetrad->x + 1, tetrad->y + 1};
        cells[3] = {tetrad->x + 2, tetrad->y + 1};
        break;
    }

    for_array(i, int, cells) {
        if (!request_cell(board, cells[i].x, cells[i].y)) {
            return false;
        }
    }
    for_array(i, int, cells) {
        tetrad->cells[i].x = cells[i].x;
        tetrad->cells[i].y = cells[i].y;
    }
    return true; 
}

bool rotate_tetrad(Tetrad *tetrad, Board board) {
    bool valid = false;
    switch (tetrad->kind) {
    case Tetrad_I:
        valid = rotate_tetrad_I(tetrad, board);
        break;
    case Tetrad_O:
        valid = true;
        break;
    case Tetrad_J:
        valid = rotate_tetrad_J(tetrad, board);
        break;
    case Tetrad_L:
        valid = rotate_tetrad_L(tetrad, board);
        break;
    case Tetrad_S:
        valid = rotate_tetrad_S(tetrad, board);
        break;
    case Tetrad_Z:
        valid = rotate_tetrad_Z(tetrad, board);
        break;
    case Tetrad_T:
        valid = rotate_tetrad_T(tetrad, board);
        break;
    }
    if (valid) {
        tetrad->dir++;
        tetrad->dir %= (int)(DOWN + 1);
    }
    return valid;
}

bool make_tetrad(Tetrad *tetrad, Board board, TetradKind kind, int x, int y) {
    Cell cells[4] = {};

    int color = 0;
    switch (kind) {
    case Tetrad_I:
        cells[0] = {x + 0, y + 1};
        cells[1] = {x + 1, y + 1};
        cells[2] = {x + 2, y + 1};
        cells[3] = {x + 3, y + 1};
        color = Cyan;
        break;
    case Tetrad_O:
        cells[0] = {x + 0, y + 0};
        cells[1] = {x + 1, y + 0};
        cells[2] = {x + 0, y + 1};
        cells[3] = {x + 1, y + 1};
        color = Yellow;
        break;
    case Tetrad_L:
        cells[0] = {x + 0, y + 1};
        cells[1] = {x + 1, y + 1};
        cells[2] = {x + 2, y + 1};
        cells[3] = {x + 2, y + 0};
        color = Orange;
        break;
    case Tetrad_J:
        cells[0] = {x + 0, y + 0};
        cells[1] = {x + 0, y + 1};
        cells[2] = {x + 1, y + 1};
        cells[3] = {x + 2, y + 1};
        color = Blue;
        break;
    case Tetrad_S:
        cells[0] = {x + 0, y + 1};
        cells[1] = {x + 1, y + 1};
        cells[2] = {x + 1, y + 0};
        cells[3] = {x + 2, y + 0};
        color = Green;
        break;
    case Tetrad_Z:
        cells[0] = {x + 0, y + 0};
        cells[1] = {x + 1, y + 0};
        cells[2] = {x + 1, y + 1};
        cells[3] = {x + 2, y + 1};
        color = Red;
        break;
    case Tetrad_T:
        cells[0] = {x + 0, y + 1};
        cells[1] = {x + 1, y + 1};
        cells[2] = {x + 2, y + 1};
        cells[3] = {x + 1, y + 0};
        color = Purple;
        break;
    }

    for (int i = 0; i < 4; i++) {
        if (!request_cell(board, x, y + i))  {
            return false;
        }
    }

    tetrad->kind = kind;
    tetrad->x = x;
    tetrad->y = y;
    for (int i = 0; i < 4; i++) {
        tetrad->cells[i] = cells[i];
        tetrad->cells[i].color = color;
    }
    tetrad->dir = LEFT;
    return true;
}

bool move_tetrad(Tetrad *tetrad, Board board, int dx, int dy) {
    Cell cells[4];
    for (int i =0; i < 4; i++) {
        Cell *cell = &cells[i];
        *cell = tetrad->cells[i];
        cell->x += dx;
        cell->y += dy;
        if (!request_cell(board, cell->x, cell->y)) {
            return false;
        }
    }

    for (int i = 0; i < 4; i++) {
        tetrad->cells[i] = cells[i];
    }

    tetrad->x += dx;
    tetrad->y += dy;
    return true;
}

bool row_is_full(Board board, int row) {
    bool full = true;
    for (int col = 0; col < board.cols; col++) {
        if (request_cell(board, col, row)) {
            full = false;
        }
    }
    return full;
}

TetradKind random_tetrad() {
    TetradKind kind = (TetradKind)GetRandomValue(1, Tetrad_Count - 1);
    return kind;
}

void draw_cell(Cell cell, Vector2 board_start, Vector2 cell_size) {
    int x = (int)board_start.x + cell.x * (int)cell_size.x;
    int y = (int)board_start.y + cell.y * (int)cell_size.y;

    const int num_tiles = 7;
    const float tex_width = tile_texture.width / (float)num_tiles;
    Rectangle tex_rect = {cell.color * tex_width, 0.0f, tex_width, (float)tile_texture.height};
    Rectangle dest_rect = {(float)x, (float)y, (float)cell_size.x, (float)cell_size.y};
    Vector2 origin = {cell_size.x/2.0f, 0};
    DrawTexturePro(tile_texture, tex_rect, dest_rect, origin, 0.0f, WHITE);
}

void draw_board(Board board, Vector2 board_start, Vector2 cell_size) {
    int board_width = board.cols * (int)cell_size.x;
    int board_height = board.rows * (int)cell_size.y;

    Color bg_color = {0x16, 0x16, 0x16, 0xff};
    DrawRectangle((int)board_start.x - (int)cell_size.x/2, (int)board_start.y, board_width, board_height, bg_color);
            
    for (int i = 0; i < board.cols * board.rows; i++) {
        Cell cell = board.cells[i];
        int col = i % board.cols;
        int row = i / board.cols;
        int x = (int)board_start.x + col * (int)cell_size.x;
        int y = (int)board_start.y + row * (int)cell_size.y;
        if (!cell.available) {
            draw_cell(cell, board_start, cell_size);
        }
    }
}

void draw_tetrad(Tetrad tetrad, Vector2 start, Vector2 cell_size) {
    for_array(i, int, tetrad.cells) {
        draw_cell(tetrad.cells[i], start, cell_size);
    }
}

void update_main_game_and_render() {

}

int main() {
    SetTraceLogLevel(LOG_ERROR);
    // SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tetris");
    SetTargetFPS(60);
    InitAudioDevice();

    Sound place_sound = LoadSound("data/place.ogg");
    tile_texture = LoadTexture("data/tiles.png");

    Board board = make_board(20, 12);

    Tetrad tetrad{};
    Tetrad hold_tetrad{};
    Tetrad next_tetrad{};
    make_tetrad(&tetrad, board, random_tetrad(), 5, 0);
    make_tetrad(&next_tetrad, board, random_tetrad(), 0, 0);

    bool tetrad_was_swapped = false;

    Camera2D camera{};

    const float move_time = 0.50f;
    float move_timer = move_time;

    int player_score = 0;

    SetRandomSeed((unsigned int)time(NULL));

    game_mode = GameMode_Menu;

    while (!WindowShouldClose()) {

        switch (game_mode) {
        case GameMode_Menu:
            break;
        case GameMode_Paused:
            break;
        case GameMode_Playing:
            update_main_game_and_render();
            break;
        case GameMode_GameOver:
            break;
        }

        // UPDATE
        if (IsKeyPressed(KEY_UP)) {
            rotate_tetrad(&tetrad, board);
        }
        if (IsKeyPressed(KEY_LEFT) || IsKeyPressedRepeat(KEY_LEFT)) {
            move_tetrad(&tetrad, board, -1, 0);
        }
        if (IsKeyPressed(KEY_RIGHT) || IsKeyPressedRepeat(KEY_RIGHT)) {
            move_tetrad(&tetrad, board, 1, 0);
        }
        if (IsKeyPressed(KEY_DOWN) || IsKeyPressedRepeat(KEY_DOWN)) {
            move_tetrad(&tetrad, board, 0, 1);
        }

        // Place tetrad down
        if (IsKeyPressed(KEY_SPACE)) {
            // NOTE: Move till can't
            while (move_tetrad(&tetrad, board, 0, 1));
            PlaySound(place_sound);
        }

        // Swap the held tetrad and the current one
        if (IsKeyPressed(KEY_LEFT_SHIFT) && !tetrad_was_swapped) {
            tetrad_was_swapped = true;
            Tetrad t = tetrad;
            tetrad = hold_tetrad;
            hold_tetrad = t;
            // NOTE: No held tetrad so use new tetrad
            if (tetrad.kind == Tetrad_Invalid) {
                tetrad = next_tetrad;
                move_tetrad(&tetrad, board, 5, 0);
                make_tetrad(&next_tetrad, board, random_tetrad(), 0, 0);
            }
            int dx = 5 - tetrad.x;
            int dy = -tetrad.y;
            move_tetrad(&tetrad, board, dx, dy);
            // NOTE: Reset to 0,0
            move_tetrad(&hold_tetrad, board, -hold_tetrad.x, -hold_tetrad.y);
            move_timer = move_time;
        }

        // Move tetrad
        move_timer -= GetFrameTime();
        if (move_timer <= 0.0f) {
            move_timer = move_time;
            if (!move_tetrad(&tetrad, board, 0, 1)) {
                // NOTE: Fill cells from tetrad that's now placed
                for_array(i, int, tetrad.cells) {
                    Cell *cell = request_cell(board, tetrad.cells[i].x, tetrad.cells[i].y);
                    cell->available = false;
                    cell->color = tetrad.cells[i].color;
                }

                tetrad_was_swapped = false;
                tetrad = next_tetrad;
                move_tetrad(&tetrad, board, 5, 0);
                make_tetrad(&next_tetrad, board, random_tetrad(), 0, 0);
            }
        }

        for (int row = board.rows - 1; row >= 0; row--) {
            if (row_is_full(board, row)) {
                for (int curr = row; curr >= 1; curr--) {
                    for (int col = 0; col < board.cols; col++) {
                        Cell *above = cell_at(board, col, curr - 1);
                        Cell *cell =  cell_at(board, col, curr);
                        cell->available = above->available;
                        cell->color     = above->color;
                    }

                    player_score += 10;
                }
            }
        }

        // RENDER
        int width = GetScreenWidth();
        int height = GetScreenHeight();
        Vector2 cell_size = {height/board.rows * 0.8f, height/board.rows * 0.8f};

        BeginDrawing();
        ClearBackground(BLACK);
        
        camera.target = {0.0f, 0.0f};
        camera.offset = {0.0f, 0.0f};
        camera.rotation = 0.0f;
        camera.zoom = 1.0f;
        BeginMode2D(camera);

        // Draw main board and current tetrad
        Vector2 board_size = {board.cols * cell_size.x, board.rows * cell_size.y};
        Vector2 board_start = {width/2.0f - board_size.x/2.0f, (height - board_size.y)/2.0f};
        draw_board(board, board_start, cell_size);
        draw_tetrad(tetrad, board_start, cell_size);

        // Draw held tetrad
        Vector2 held_start = {0.25f*board_start.x, 0.0f};
        Vector2 held_box_size = {100.0f, 100.0f};
        if (hold_tetrad.kind != Tetrad_Invalid) {
            draw_tetrad(hold_tetrad, held_start, held_box_size);
        }
        
        // Draw next tetrad(s)
        Vector2 next_box_size = { width - 1.10f * (board_start.x + board_size.x), height / 2.0f };
        Vector2 next_box_start = {1.10f * (board_start.x + board_size.x), height / 2.0f };
        // Vector2 next_box_start = {1000.0f, 200.0f};
        Vector2 next_size = {next_box_size.x/8.0f, next_box_size.x/8.0f};
        draw_tetrad(next_tetrad, next_box_start, next_size);

        char score_text[10];
        snprintf(score_text, sizeof(score_text), "%d", player_score);
        DrawText(score_text, 0, 0, 40, WHITE);

        EndMode2D();
        EndDrawing();
    }
    
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
