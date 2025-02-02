#ifndef BATTLE_H
#define BATTLE_H

#include "essentials.h"
#include "game_menu.h"
#include "map.h"
void init_demon(Monster *demon);
void place_monster(char map[MAX_ROW][COLS], Room room[MAX_ROOMS], Monster *monster);
bool is_in_room(Room *room, int x, int y );
bool is_passable(char map[MAX_ROW][COLS], int y, int x);
void move_monster(char map[MAX_ROW][COLS],Room room[MAX_ROOMS], Monster *monster);
void monster_attack( Monster *demon);
void monster_take_damage(Monster *demon, int damage);
void check_game_over();
void lost();
void won();

//general
void place_monster(char map[MAX_ROW][COLS], Room room[MAX_ROOMS], Monster *monster) {
    int which_room = rand() % 6;
    // Ensure the treasure is not placed in the same room as stairs
    if (which_room == player.first_room_pos) {
        which_room = (which_room + 1) % 6;
    }
    if (which_room == player.forward_stair_pos) {
        which_room = (which_room + 1) % 6;
    }

    int y = room[which_room].y + rand_range(1,room[which_room].height - 2) ;
    int x = room[which_room].x + rand_range(1, room[which_room].width - 2) ;

    // if ((mvinch(y, x) & A_CHARTEXT) != '.') {
    if (map[y][x] != '.'){
        // If not, recursively try again
        place_monster(map, room, monster);
//        room[which_room].type = 3;	//treasure room
        return;
    }

    monster->pos.y = y;
    monster->pos.x = x;
    mvaddch(y,x, 'D');
    map[y][x] = 'D';
    monster->room = which_room;
}
bool is_in_room(Room *room, int x, int y ) {
    for (int i = 0; i < MAX_ROOMS; i++) {  // Loop through all rooms
        // Check if player's coordinates are inside the room boundaries
        if (x >= room->x && x < room->x + room->width &&
            y >= room->y && y < room->y + room->height) {
            return true;  // Player is inside this room
            }
    }
    return false;  // Player is not inside any room
}
bool is_passable(char map[MAX_ROW][COLS], int y, int x) {
    // Get the character at the specified position
    chtype tile = mvwinch(stdscr, y, x);
    //int tile = map[y][x];

    // Check if the tile is a wall or door
    return (tile != '-' && tile != '_' && tile != '|' && tile != '+' && tile != '@');
}
void move_monster(char map[MAX_ROW][COLS],Room room[MAX_ROOMS], Monster *monster) {
    int old_item = map[monster->pos.y][monster->pos.x];
    int new_y = monster->pos.y;
    int new_x = monster->pos.x;

    // If the player is in the room, the Demon starts moving toward them
    if (is_in_room(&room[monster->room], player.player_pos.x, player.player_pos.y) ){
        int dx = (player.player_pos.x > monster->pos.x) ? 1 : (player.player_pos.x < monster->pos.x) ? -1 : 0;
        int dy = (player.player_pos.y > monster->pos.y) ? 1 : (player.player_pos.y < monster->pos.y) ? -1 : 0;

        // Demon tries to move toward the player in x or y direction
        if (dx != 0) {
            new_x += dx;
        } else if (dy != 0) {
            new_y += dy;
        }

    }
    // Check if the new position is within the room boundaries and passable
    if (new_y >= room[monster->room].y && new_y < room[monster->room].y + room[monster->room].height &&
        new_x >= room[monster->room].x && new_x < room[monster->room].x + room[monster->room].width &&
        is_passable(map, new_y, new_x)) {
        // Update the Demon's position
        map[monster->pos.y ][monster->pos.x] = '.';
        monster->pos.y = new_y;
        monster->pos.x = new_x;
        map[monster->pos.y ][monster->pos.x] = 'D';

        }
}
void monster_attack( Monster *monster) {
    // Check if the monster is adjacent to the player (horizontally or vertically)
    if ((abs(player.player_pos.x - monster->pos.x) == 1 && player.player_pos.y == monster->pos.y) ||
        (abs(player.player_pos.y - monster->pos.y) == 1 && player.player_pos.x == monster->pos.x)) {
        // Decrease player health by 5
        player.health -= monster->attack;
        mvprintw(0, 0, "monster heat you! Watch yourself!", player.health);
        }
}
void monster_take_damage(Monster *monster, int damage) {
    monster->health -= damage;
    if (monster->health <= 0) {
        mvprintw(0, 0, "The monster has been defeated!");
        // Handle monster death (remove from screen, etc.)
    }
}

//Demon
void init_demon(Monster *demon) {
    demon->symbol = 'D';
    demon->health = 5;  // Weakest monster
    demon->attack = 5;
    demon->defense = 5;
    demon->speed = 2;
}

//Fire Breathing
void init_fireb(Monster *fireb) {
    fireb->symbol = 'F';
    fireb->health = 10;  // second Weakest
    fireb->attack = 10;
    fireb->defense = 5;
    fireb->speed = 2;
}

//Giant: follows player for 5 steps
void init_giant(Monster *monster) {
    monster->symbol = 'G';
    monster->health = 15;  // third weakest
    monster->attack = 10;
    monster->defense = 5;
    monster->speed = 2;
}

//Snake: follows player untill it is alive
void init_snake(Monster *monster) {
    monster->symbol = 'S';
    monster->health = 20;
    monster->attack = 15;
    monster->defense = 5;
    monster->speed = 2;
}

//Undeed: follows player for 5 steps
void init_undeed(Monster *monster) {
    monster->symbol = 'U';
    monster->health = 30;  // strongest
    monster->attack = 20;
    monster->defense = 5;
    monster->speed = 2;
}



void check_game_over() {
    if (player.health <= 0) {
        player.lost = true;
        return;
    }
}

void lost() {
    clear();
    refresh();

    // Create a centered window for the lost_win
    int win_width = 55, win_height = DISPLAY_ROWS + 15;  // Extra space for headers & footer
    int startx = (COLS - win_width) / 2;
    int starty = (LINES - win_height) / 2;
    WINDOW *lost_win = newwin(win_height, win_width, starty, startx);
    keypad(lost_win, TRUE);

    box(lost_win, 0, 0);
    wattron(lost_win, COLOR_PAIR(1));
    mvwprintw(lost_win, 1, (win_width - 14) / 2, "YOU LOST!");
    mvwprintw(lost_win, 3, (win_width - 14) / 2, "Your Score: %d", player.score);

    wattroff(lost_win, COLOR_PAIR(1));
    wrefresh(lost_win);
    mvwaddch(lost_win, 2, 0, ACS_LTEE);
    mvwhline(lost_win, 2, 1, ACS_HLINE, win_width - 2);
    mvwaddch(lost_win, 2, win_width - 1, ACS_RTEE);


}

#endif //BATTLE_H
