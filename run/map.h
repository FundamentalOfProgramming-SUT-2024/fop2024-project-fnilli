#ifndef MAP_H
#define MAP_H

#include "essentials.h"





int rand_range(int min, int max);
void generate_rooms(Room rooms[]);
bool rooms_overlap(Room *a, Room *b);
void connect_rooms(Room *a, Room *b);
void draw_room(Room *room, int room_index);
void connect_rooms(Room *a, Room *b) ;
void draw_corridor(int x1, int y1, int x2, int y2) ;


int rand_range(int min, int max) {
    return min + rand() % (max - min + 1);
}

void generate_rooms(Room room[] ) {

    int region_width = MAX_COL / 2;
    int region_height = (PLAYABLE_BOTTOM - PLAYABLE_TOP) / 3;


    for (int i = 0; i < 6; ++i) {

        //split screen to 6 regions
        int region_x = (i % 2) * region_width;
        // int region_y = (i / 2) * region_height;
        int region_y = PLAYABLE_TOP + (i / 2) * region_height; // Offset by reserved rows

        while (true) {
            room[i].width = MIN_ROOM_SIZE + rand() % (MAX_ROOM_SIZE - MIN_ROOM_SIZE + 1);
            room[i].height = MIN_ROOM_SIZE + rand() % (MAX_ROOM_SIZE - MIN_ROOM_SIZE + 1);
            //دو خط زیر هر اتاق را در یکی از 6 بخش نقشه درست میکند
            room[i].x = rand_range(region_x, region_x + region_width - room[i].width);      //room made for splited screen
            room[i].y = rand_range(region_y, region_y + region_height - room[i].height);    //room made for splited screen


            // Check for overlap with existing rooms
            bool overlaps = false;
            for (int j = 0; j < i; ++j) {
                if (rooms_overlap(&room[i], &room[j])) {
                    overlaps = true;
                    break;
                }
            }
            if (!overlaps) break;
        }
    }
}

bool rooms_overlap(Room *a, Room *b) {
    return !(a->x + a->width < b->x || b->x + b->width < a->x ||
             a->y + a->height < b->y || b->y + b->height < a->y);
}
void draw_room(Room *room, int room_index) {
    if (room->y < PLAYABLE_TOP || room->y + room->height > PLAYABLE_BOTTOM) return; // Skip if out of bounds

    for (int x = room->x; x < room->x + room->width; ++x) {
        if (room->y >= PLAYABLE_TOP) mvaddch(room->y, x, '_'); // Top wall
        if (room->y + room->height - 1 < PLAYABLE_BOTTOM) mvaddch(room->y + room->height - 1, x, '_'); // Bottom wall
    }
    for (int y = room->y + 1; y < room->y + room->height - 1; ++y) {
        if (room->x >= 0) mvaddch(y, room->x, '|'); // Left wall
        if (room->x + room->width - 1 < MAX_COL) mvaddch(y, room->x + room->width - 1, '|'); // Right wall
    }

    for (int y = room->y + 1; y < room->y + room->height - 1; ++y) {
        for (int x = room->x + 1; x < room->x + room->width - 1; ++x) {
            mvaddch(y, x, '.'); // Floor
        }
    }
}
// void draw_room(Room *room, int room_index) {
//     // Draw the walls
//     for (int x = room->x; x < room->x + room->width; ++x) {
//         mvaddch(room->y, x, '_'); // Top wall
//         mvaddch(room->y + room->height - 1, x, '_'); // Bottom wall
//     }
//     for (int y = room->y + 1; y < room->y + room->height; ++y) {
//         mvaddch(y, room->x, '|'); // Left wall
//         mvaddch(y, room->x + room->width - 1, '|'); // Right wall
//     }
//
//     // Fill the floor
//     for (int y = room->y + 1; y < room->y + room->height - 1; ++y) {
//         for (int x = room->x + 1; x < room->x + room->width - 1; ++x) {
//             mvaddch(y, x, '.');
//         }
//     }
//
// }

void connect_rooms(Room *a, Room *b) {

    // get a cordination for each room's door
    // int x1 = a->x + (rand() % a->width) ;
    // int y1 = a->y + (rand() % a->height);
    // int x2 = b->x + (rand() % b->width) ;
    // int y2 = b->y + (rand() % b->height);
    int x1 = a->x + rand_range(1, a->width - 2);
    int y1 = a->y + rand_range(1, a->height - 2);
    int x2 = b->x + rand_range(1, b->width - 2);
    int y2 = b->y + rand_range(1, b->height - 2);

    // Draw a corridor between the centers
    draw_corridor(x1, y1, x2, y2);
}


// void drawww_corridor(int x1, int y1, int x2, int y2) {
//
//     int oncex=0, oncey=0;
//     //draw horizontal part of the corridor
//     while (x1 != x2) {
//         if(x1 < x2){
//             x1++;
//         }
//         else{
//             x1--;
//         }
//
//         if (mvinch(y1, x1) == '_' || mvinch(y1, x1) == '|') {
//             mvprintw(y1, x1, "+");
//             // if (!oncex) {
//             //     mvprintw(y1, x1, "+");
//             //     oncex = 1;
//             // }
//             // if (y1 > 0 && mvinch(y1 - 1, x1) == ' ') {
//             //     mvaddch(y1 - 1, x1, '#'); //place corridor above
//             // } else if (y1 < LINES - 1 && mvinch(y1 + 1, x1) == ' ') {
//             //     mvaddch(y1 + 1, x1, '#'); //place corridor below
//             // }
//         }
//         else if(mvinch(y1, x1)  == ' '){ //check if the cell is empty
//             mvprintw(y1, x1, "#");
//         }
//
//     }
//
//     // Draw vertical part of the corridor
//     while(y1 != y2){
//         if(y1 < y2){
//             y1++;
//         }
//         else{
//             y1--;
//         }
//
//         if(mvinch(y1, x1) == '|' || mvinch(y1, x1) == '_'){
//             if (!oncey) {
//                 mvprintw(y1, x1, "+");
//                 oncey = 1;
//             }
//             if (x2 > 0 && mvinch(y1, x2 - 1) == ' ') {
//                 mvaddch(y1, x2 - 1, '#'); // Place corridor to the left
//             } else if (x2 < COLS - 1 && mvinch(y1, x2 + 1) == ' ') {
//                 mvaddch(y1, x2 + 1, '#'); // Place corridor to the right
//             }
//         }
//         else if(mvinch(y1, x1) == ' '){  // Check if the cell is empty
//             mvprintw(y1, x1, "#");
//         }
//     }
// }

void draw_corridor(int x1, int y1, int x2, int y2) {

    if (y1 < PLAYABLE_TOP || y1 >= PLAYABLE_BOTTOM || y2 < PLAYABLE_TOP || y2 >= PLAYABLE_BOTTOM) return;
    // Draw horizontal corridor
    int start_x = x1 < x2 ? x1 : x2; // Start from the leftmost point
    int end_x = x1 < x2 ? x2 : x1;   // End at the rightmost point
    for (int x = start_x; x <= end_x; ++x) {
        // Check if the current position is a room wall (| or _)
        char ch = mvwinch(stdscr, y1, x);
        if (ch == '|' || ch == '_') {
            // Place a door (+) where the corridor connects to the room wall
            mvaddch(y1, x, '+');
        } else if (ch == ' ' || ch == '#') {
            // Draw the corridor floor (#) in empty spaces or over existing corridors
            mvaddch(y1, x, '#');
        }
    }

    // Draw vertical corridor
    int start_y = y1 < y2 ? y1 : y2; // Start from the topmost point
    int end_y = y1 < y2 ? y2 : y1;   // End at the bottommost point
    for (int y = start_y; y <= end_y; ++y) {
        // Check if the current position is a room wall (| or _)
        char ch = mvwinch(stdscr, y, x2);
        if (ch == '|' || ch == '_') {
            // Place a door (+) where the corridor connects to the room wall
            mvaddch(y, x2, '+');
        } else if (ch == ' ' || ch == '#') {
            // Draw the corridor floor (#) in empty spaces or over existing corridors
            mvaddch(y, x2, '#');
        }
    }
}

#endif //MAP_H
