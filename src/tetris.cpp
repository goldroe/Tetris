// TODO: Multiplayer client-server
// TODO: Particles and lighting
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

Color cell_color_to_color(int col) {
    switch (col) {
    case Yellow:
        return YELLOW;
    case Red:
        return RED;
    case Green:
        return GREEN;
    case Purple:
        return PURPLE;
    case Orange:
        return ORANGE;
    case Cyan:
        return {0x0, 0xd7, 0xfc, 0xff};
    case Blue:
        return BLUE;
    default:
        return WHITE;
    }
}

void draw_cell_outline(Cell cell, Vector2 board_start, Vector2 cell_size) {
    int x = (int)board_start.x + cell.x * (int)cell_size.x;
    int y = (int)board_start.y + cell.y * (int)cell_size.y;

    Rectangle rect = {(float)x - cell_size.x/2.0f, (float)y, (float)cell_size.x, (float)cell_size.y};
    
    Color color = cell_color_to_color(cell.color);
    DrawRectangleLinesEx(rect, 1.0f, color);
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

void update_menu_and_render(GameState *game_state) {
    Vector2 mouse_position = GetMousePosition();
    Rectangle mouse_rect = {mouse_position.x, mouse_position.y, 1.0f, 1.0f};

    int screen_width = GetScreenWidth();
    Vector2 play_rect_size = {0.5f * screen_width, 100.0f};
    Vector2 play_rect_pos = {(screen_width - play_rect_size.x)/2.0f, 200.0f};

    Rectangle play_rect = {play_rect_pos.x, play_rect_pos.y, play_rect_size.x, play_rect_size.y};
    Rectangle quit_rect = {(screen_width - (0.5f*screen_width))/2.0f, 400.0f, 0.5f*screen_width, 100.0f};

    Color play_color = {0x4b, 0x4b, 0x4b, 0xff};
    Color quit_color = play_color;

    if (CheckCollisionRecs(mouse_rect, play_rect)) {
        play_color = {0x2b, 0x2b, 0x2b, 0xff};
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            game_state->mode = GameMode_Playing;
        }
    }

    if (CheckCollisionRecs(mouse_rect, quit_rect)) {
        quit_color = {0x2b, 0x2b, 0x2b, 0xff};
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            game_state->mode = GameMode_Quit;
        }
    }
    

    DrawRectangleV(play_rect_pos, play_rect_size, play_color);
    DrawText("Play", (int)play_rect_pos.x, (int)play_rect_pos.y, 80, BLACK);
    DrawRectangleRec(quit_rect, quit_color);
    DrawText("Quit", (int)quit_rect.x, (int)quit_rect.y, 80, BLACK);
}

void update_game_and_render(GameState *game_state) {
    if (IsKeyPressed(KEY_ESCAPE)) {
        game_state->mode = GameMode_Paused;
        return;
    }
    
    if (IsKeyPressed(KEY_UP)) {
        rotate_tetrad(&game_state->tetrad, game_state->board);
    }
    if (IsKeyPressed(KEY_LEFT) || IsKeyPressedRepeat(KEY_LEFT)) {
        move_tetrad(&game_state->tetrad, game_state->board, -1, 0);
    }
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressedRepeat(KEY_RIGHT)) {
        move_tetrad(&game_state->tetrad, game_state->board, 1, 0);
    }
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressedRepeat(KEY_DOWN)) {
        move_tetrad(&game_state->tetrad, game_state->board, 0, 1);
    }

    // Place tetrad down
    if (IsKeyPressed(KEY_SPACE)) {
        // NOTE: Move till can't
        while (move_tetrad(&game_state->tetrad, game_state->board, 0, 1));
        // PlaySound(place_sound);
    }
    
    // Swap the held tetrad and the current one
    if (IsKeyPressed(KEY_LEFT_SHIFT) && !game_state->tetrad_swapped) {
        game_state->tetrad_swapped = true;
        Tetrad t = game_state->tetrad;
        game_state->tetrad = game_state->held_tetrad;
        game_state->held_tetrad = t;
        // NOTE: No held tetrad so use new tetrad
        if (game_state->tetrad.kind == Tetrad_Invalid) {
            game_state->tetrad = game_state->next_tetrad;
            move_tetrad(&game_state->tetrad, game_state->board, 5, 0);
            make_tetrad(&game_state->next_tetrad, game_state->board, random_tetrad(), 0, 0);
        }
        int dx = 5 - game_state->tetrad.x;
        int dy = -game_state->tetrad.y;
        move_tetrad(&game_state->tetrad, game_state->board, dx, dy);
        // NOTE: Reset to 0,0
        move_tetrad(&game_state->held_tetrad, game_state->board, -game_state->held_tetrad.x, -game_state->held_tetrad.y);
        game_state->move_timer = game_state->move_t;
    }

    // Move tetrad
    game_state->move_timer -= GetFrameTime();
    if (game_state->move_timer <= 0.0f) {
        game_state->move_timer = game_state->move_t;
        if (!move_tetrad(&game_state->tetrad, game_state->board, 0, 1)) {
            // NOTE: Fill cells from tetrad that's now placed
            for_array(i, int, game_state->tetrad.cells) {
                Cell *cell = request_cell(game_state->board, game_state->tetrad.cells[i].x, game_state->tetrad.cells[i].y);
                cell->available = false;
                cell->color = game_state->tetrad.cells[i].color;
            }

            game_state->tetrad_swapped = false;
            game_state->tetrad = game_state->next_tetrad;
            move_tetrad(&game_state->tetrad, game_state->board, 5, 0);
            make_tetrad(&game_state->next_tetrad, game_state->board, random_tetrad(), 0, 0);
        }
    }

    for (int row = game_state->board.rows - 1; row >= 0; row--) {
        if (row_is_full(game_state->board, row)) {
            for (int curr = row; curr >= 1; curr--) {
                for (int col = 0; col < game_state->board.cols; col++) {
                    Cell *above = cell_at(game_state->board, col, curr - 1);
                    Cell *cell =  cell_at(game_state->board, col, curr);
                    cell->available = above->available;
                    cell->color     = above->color;
                }

                game_state->player_score += 10;
            }
        }
    }

    // Get holo tetrad that shows next move on board
    Tetrad holo_tetrad = game_state->tetrad;
    while (move_tetrad(&holo_tetrad, game_state->board, 0, 1));
    
    // RENDER
    int width = GetScreenWidth();
    int height = GetScreenHeight();
    Vector2 cell_size = {height/game_state->board.rows * 0.8f, height/game_state->board.rows * 0.8f};

    Camera2D camera{};
    camera.target = {0.0f, 0.0f};
    camera.offset = {0.0f, 0.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    BeginMode2D(camera);

    // Draw main board and current tetrad
    Vector2 board_size = {game_state->board.cols * cell_size.x, game_state->board.rows * cell_size.y};
    Vector2 board_start = {width/2.0f - board_size.x/2.0f, (height - board_size.y)/2.0f};
    draw_board(game_state->board, board_start, cell_size);

    for_array(i, int, holo_tetrad.cells) {
        draw_cell_outline(holo_tetrad.cells[i], board_start, cell_size);
    }
    draw_tetrad(game_state->tetrad, board_start, cell_size);

    // Draw held tetrad
    Vector2 held_start = {0.25f*board_start.x, 0.0f};
    Vector2 held_box_size = {100.0f, 100.0f};
    if (game_state->held_tetrad.kind != Tetrad_Invalid) {
        draw_tetrad(game_state->held_tetrad, held_start, held_box_size);
    }
        
    // Draw next tetrad(s)
    Vector2 next_box_size = { width - 1.10f * (board_start.x + board_size.x), height / 2.0f };
    Vector2 next_box_start = {1.10f * (board_start.x + board_size.x), height / 2.0f };
    // Vector2 next_box_start = {1000.0f, 200.0f};
    Vector2 next_size = {next_box_size.x/8.0f, next_box_size.x/8.0f};
    draw_tetrad(game_state->next_tetrad, next_box_start, next_size);

    char score_text[10];
    snprintf(score_text, sizeof(score_text), "%d", game_state->player_score);
    DrawText(score_text, 0, 0, 40, WHITE);

    EndMode2D();
}

void update_pause_menu_and_render(GameState *game_state) {
    Vector2 mouse_pos = GetMousePosition();
    Rectangle mouse_rect = {mouse_pos.x, mouse_pos.y, 1, 1};
    int width = GetScreenWidth();
    int height = GetScreenHeight();
    Rectangle continue_rect = {0.25f*width, 100.0f, width/2.0f, 100.0f};
    Rectangle quit_rect = {0.25f*width, 300.0f, width/2.0f, 100.0f};
    Color continue_color = {0x4b, 0x4b, 0x4b, 0xff};
    Color quit_color = {0x4b, 0x4b, 0x4b, 0xff};
    
    if (CheckCollisionRecs(mouse_rect, continue_rect)) {
        continue_color = {0x2b, 0x2b, 0x2b, 0xff};
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            game_state->mode = GameMode_Playing;
        }
    }
    if (CheckCollisionRecs(mouse_rect, quit_rect)) {
        quit_color = {0x2b, 0x2b, 0x2b, 0xff};
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            game_state->mode = GameMode_Quit;
        }
    }
    
    DrawRectangleRec(continue_rect, continue_color);
    DrawText("Continue", (int)continue_rect.x, (int)continue_rect.y, 40, WHITE);
    DrawRectangleRec(quit_rect, quit_color);
    DrawText("Quit", (int)quit_rect.x, (int)quit_rect.y, 40, WHITE);
}

int main() {
    SetTraceLogLevel(LOG_ERROR);
    // SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tetris");
    SetTargetFPS(60);
    SetExitKey(KEY_NULL);
    InitAudioDevice();

    Sound place_sound = LoadSound("data/place.ogg");
    tile_texture = LoadTexture("data/tiles.png");

    Camera2D camera{};

    SetRandomSeed((unsigned int)time(NULL));

    GameState game_state{};
    game_state.mode = GameMode_Menu;
    game_state.board = make_board(20, 12);

    make_tetrad(&game_state.tetrad, game_state.board, random_tetrad(), 5, 0);
    make_tetrad(&game_state.next_tetrad, game_state.board, random_tetrad(), 0, 0);
    game_state.tetrad_swapped = false;
    game_state.move_t = 0.5f;
    game_state.move_timer = game_state.move_t;

    bool game_is_running = true;    
    while (game_is_running) {
        game_is_running = !WindowShouldClose() && (game_state.mode != GameMode_Quit);

        BeginDrawing();
        ClearBackground(BLACK);
        
        switch (game_state.mode) {
        case GameMode_Menu:
            update_menu_and_render(&game_state);
            break;
        case GameMode_Paused:
            update_pause_menu_and_render(&game_state);
            break;
        case GameMode_Playing:
            update_game_and_render(&game_state);
            break;
        case GameMode_GameOver:
            break;
        }
    
        EndDrawing();
    }
    
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
