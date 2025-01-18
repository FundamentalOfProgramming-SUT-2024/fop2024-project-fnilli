#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define MAX_ROOMS 6
#define MIN_ROOM_SIZE 5
#define MAX_ROOM_SIZE 16


typedef struct {
    int x, y;      // Top-left corner
    int width;     // Room width
    int height;    // Room height
} Room;

// Function prototypes
int rand_range(int min, int max);
void generate_rooms(Room rooms[]);
bool rooms_overlap(Room *a, Room *b);
void connect_rooms(Room *a, Room *b);
void draw_room(Room *room, int room_index);
void draw_corridor(int x1, int y1, int x2, int y2);

int main() {
    initscr();            // Initialize ncurses
    noecho();             // Disable echoing
    cbreak();             // Disable line buffering
    curs_set(0);          // Hide the cursor
    srand(time(NULL));    // Seed for random number generation

    Room room[MAX_ROOMS];
    generate_rooms(room);

    // Draw all rooms and connect them
    for (int i = 0; i < MAX_ROOMS; ++i) {
        draw_room(&room[i], i);
        if (i > 0) {
//            connect_rooms(&room[i - 1], &room[i]);
        }
    }

    // Wait for user input to exit
    getch();
    endwin();
    return 0;
}

int rand_range(int min, int max) {
    return min + rand() % (max - min + 1);
}

void generate_rooms(Room room[] ) {

    int region_width = COLS / 2;
    int region_height = LINES / 3;


    for (int i = 0; i < 6; ++i) {

        //split screen to 6 regions
        int region_x = (i % 2) * region_width;
        int region_y = (i / 2) * region_height;


        while (true) {
            room[i].width = MIN_ROOM_SIZE + rand() % (MAX_ROOM_SIZE - MIN_ROOM_SIZE + 1);
            room[i].height = MIN_ROOM_SIZE + rand() % (MAX_ROOM_SIZE - MIN_ROOM_SIZE + 1);
            // room[i].x = rand() % (COLS - room[i].width - 1);
            // room[i].y = rand() % (LINES - room[i].height - 1);
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
    // Draw the walls
    for (int x = room->x; x < room->x + room->width; ++x) {
        mvaddch(room->y, x, '_'); // Top wall
        mvaddch(room->y + room->height - 1, x, '_'); // Bottom wall
    }
    for (int y = room->y + 1; y < room->y + room->height; ++y) {
        mvaddch(y, room->x, '|'); // Left wall
        mvaddch(y, room->x + room->width - 1, '|'); // Right wall
    }

    // Fill the floor
    for (int y = room->y + 1; y < room->y + room->height - 1; ++y) {
        for (int x = room->x + 1; x < room->x + room->width - 1; ++x) {
            mvaddch(y, x, '.');
        }
    }
    // Place a door
    int door_side, door_side2 = -1;

    do { //در دو اتاق بالایی، در ضلع بالای انها نباشد و در دو اتاق پایینی، در ضلع پایین انها نباشد

        door_side = rand() % 4; // 0: top, 1: bottom, 2: left, 3: right

        // If the room is one of the two uppermost rooms, skip placing doors on the top wall
        if ((room_index == 0 || room_index == 1) && door_side == 0) {
            continue;
        }
        else if ((room_index == 4 || room_index == 5) && door_side ==  1) {
            continue;
        }
        else if (room_index == 2 || room_index == 3) {
            do {
                door_side2 = rand() % 4;  // 0: top, 1: bottom, 2: left, 3: right

            }while (door_side2 == door_side);

        }
        break; // Valid door_side selected
    } while (true);


    //draw door
    if (door_side == 0) {
        mvaddch(room->y, room->x + 1 + rand() % (room->width - 2), '+');
    } else if (door_side == 1) {
        mvaddch(room->y + room->height - 1, room->x + 1 + rand() % (room->width - 2), '+');
    } else if (door_side == 2) {
        mvaddch(room->y + 1 + rand() % (room->height - 2), room->x, '+');
    } else {
        mvaddch(room->y + 1 + rand() % (room->height - 2), room->x + room->width - 1, '+');
    }

    //draw second door
    if (room_index == 2 || room_index == 3) {
        if (door_side2 == 0) {
            mvaddch(room->y, room->x + 1 + rand() % (room->width - 2), '+');
        } else if (door_side2 == 1) {
            mvaddch(room->y + room->height - 1, room->x + 1 + rand() % (room->width - 2), '+');
        } else if (door_side2 == 2) {
            mvaddch(room->y + 1 + rand() % (room->height - 2), room->x, '+');
        } else {
            mvaddch(room->y + 1 + rand() % (room->height - 2), room->x + room->width - 1, '+');
        }
    }

}


