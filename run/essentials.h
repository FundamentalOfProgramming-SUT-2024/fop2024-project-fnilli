#ifndef ESSENCIALS_H
#define ESSENCIALS_H

// #include <ncursesw/curses.h>
#include <locale.h>
// #include <wchar.h>

#include <ncursesw/ncurses.h>   // Use the wide-character ncurses
#include <ncursesw/menu.h>      // Use the wide-character menu library

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
#define MAX_LINE 100
#define MAX_USERNAME 50
#define MAX_USERS 100
#define MAX_NAME_LEN 30
#define DISPLAY_ROWS 10  // Number of visible rows in the scoreboard

#define MAX_PLAYERS 100
#define MAX_LINE_LENGTH 256



#define MAX_ROOMS 6
#define MIN_ROOM_SIZE 5
#define MAX_ROOM_SIZE 15
// Define boundaries for the terminal
#define MAX_COL COLS           // Number of columns in the terminal
#define MAX_ROW LINES           // Number of rows in the terminal
#define RESERVED_TOP 1         // Reserved rows at the top
#define RESERVED_BOTTOM 4      // Reserved rows at the bottom
#define PLAYABLE_TOP RESERVED_TOP
#define PLAYABLE_BOTTOM (MAX_ROW - RESERVED_BOTTOM)


typedef struct {
    int x;
    int y;
} Pos;


typedef struct {
    int x, y;      // Top-left corner
    int width; //x
    int height;//y
    bool visited;

    int type;  //1: regular  2: spell  3:treasure  4: nightmare
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
    int game_difficulty;    //3: easy(3 gold each floor)   2: medim(2 gold each floor)    3: hard(1 gold each floor)
    int player_color;
    int song;
    int floor;
    int forward_stair_pos;
    int backward_stair_pos;
    bool a_pressed;
    bool map_revealed;
    int first_room_pos;
    Pos player_pos;
    int health;
    int food_count;

    bool lost;
    bool won;



    Room rooms[6];
    int MAX_health;
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
    int k_lock;
} Player;

typedef struct {
    Pos pos;
    char symbol;
    int health; //how many times it should be damaged
    int attack; //how much it decreases playe's health
    int defense;
    int speed;
    bool alive;
    int room;
} Monster;



#endif //ESSENCIALS_H
