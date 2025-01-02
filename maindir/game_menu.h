#ifndef GAME_MENU_H
#define GAME_MENU_H

#include <ncurses.h>
#include <menu.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>

// Constants
#define ARRAY_SIZE(a) (sizeof(*a) / sizeof(a[0]))
#define MAX_LENGTH 100

// Menu choices
extern char *choices[];

// Function declarations
void print_in_middle(WINDOW *win, int starty, int startx, int width, char *string, chtype color);

void create_new_rogue(MENU *menu, WINDOW *menu_win);
bool username_exists(const char *filename, const char *username);
void register_user(const char *filename, const char *username, const char *password);
bool contains_number(const char *password);
bool contains_lowercase(const char *password);
bool contains_uppercase(const char *password);
bool is_valid_email(const char *email);
char *generate_random_password();

void login_rogue(MENU *menu, WINDOW *menu_win);
bool validate_credentials(const char *filename, const char *username, const char *password);

void before_you_play(MENU *menu, WINDOW *menu_win);

#endif //GAME_MENU_H
