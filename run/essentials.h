#ifndef ESSENCIALS_H
#define ESSENCIALS_H

// #include <ncursesw/curses.h>
// #include <locale.h>
// #include <wchar.h>
#include <ncurses.h>
#include <menu.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>


#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define MAX_LENGTH 100

#define MAX_ROOMS 6
#define MIN_ROOM_SIZE 4
#define MAX_ROOM_SIZE 13
// Define boundaries for the terminal
#define MAX_COL COLS           // Number of columns in the terminal
#define MAX_ROW LINES           // Number of rows in the terminal
#define RESERVED_TOP 1         // Reserved rows at the top
#define RESERVED_BOTTOM 5      // Reserved rows at the bottom
#define PLAYABLE_TOP RESERVED_TOP
#define PLAYABLE_BOTTOM (MAX_ROW - RESERVED_BOTTOM)


typedef struct {
    int x;
    int y;
} Pos;


typedef struct {
    int x, y;      // Top-left corner
    int width;
    int height;
    bool visitied;


    int type;
    int locked;
    int password;
    Pos doors[2];
    Pos pillar_seed;
    int traps_count;
    Pos traps[10];
    int gold;
    int dark_gold;
    int ordinary_food;
} Room;

typedef struct {
    char username[50];
    char password[50];
    char email[50];
    int score;
    int gold;
    int count_games;
    int experience; //count of ended games
    bool guest;
    bool logged_in;
    bool resume_game_bool;
    bool creat_game_bool;
    int game_difficulty;
    int player_color;
    int song;



    Pos player_pos;
    Room rooms[6];
    int MAX_health;
    int health;
    int floor_number;
    int hungriness;
    int players_ordinary_food;
    int players_mace;
    int players_dagger;
    int players_magic_wand;
    int players_arrow;
    int players_sword;
    int players_health_potion;
    int players_speed_potion;
    int players_damage_potion;
    time_t start_time;
    int k_lock;
    time_t password_start_time;
} Player;

int rand_range(int min, int max) {
    return min + rand() % (max - min + 1);
}

#endif //ESSENCIALS_H
