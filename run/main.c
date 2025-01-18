
#include "game_menu.h"
#include "map.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define MAX_LENGTH 100



typedef struct{
    int x, y;
} Pos;


void start_ncurses();
void start_colors();
void start_game();
void start_menu();
void start_map(Room room[]);
void start_player_position(Pos *player, Room room[]) ;
void handle_movement(int ch, Pos *p, char map[LINES][COLS]);
bool is_corridor(char map[LINES][COLS],Pos player);
bool is_room(char map[LINES][COLS],Pos player);


void save_screen_to_array(char map[LINES][COLS]);
void print_map_from_array(char map[LINES][COLS]) ;
void save_map_to_file(const char *filename, char map[LINES][COLS]);
void load_map_from_file(const char *filename, char map[LINES][COLS]);


int main() {
    Pos player;

    start_ncurses();
    start_game();
    start_menu();

    curs_set(FALSE);
    Room room[MAX_ROOMS];
    start_map(room);
    char map[LINES][COLS];
    save_screen_to_array(map);
    save_map_to_file("map.txt", map);
    start_player_position(&player, room);

    while (1) {
        //DRAW MAP
        print_map_from_array(map);
        //DRAW PLAYER
        attron(COLOR_PAIR(3));
        noecho();
        mvaddch(player.y, player.x, '@');
        attroff(COLOR_PAIR(3));
        //MOVE PLAYER
        int ch = getch();
        handle_movement(ch, &player, map);

    }


}

void start_ncurses() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    start_colors();
    srand(time(NULL));
}
void start_colors() {
    start_color();
    init_pair(1, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(2, COLOR_CYAN, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_BLACK);
    init_pair(4, COLOR_GREEN, COLOR_BLACK);
    init_pair(5, COLOR_BLUE, COLOR_BLACK);
    init_pair(6, COLOR_WHITE, COLOR_BLACK);
    init_pair(7, COLOR_YELLOW, COLOR_BLACK);
    int gold = 8;
    init_color(gold, 1000, 840, 0);
    init_pair(8, gold, COLOR_BLACK);
}
void start_game() {
    //show my name
    attron(A_BOLD |  COLOR_PAIR(1));
    char message[] = "ROGUE";
    mvprintw((LINES / 2)-1, (COLS - strlen(message)) / 2, "%s", message); // نمایش پیام در مرکز
    attroff(A_BOLD);

    char myname[] = "Fatemeh Nilforoushan";
    mvprintw((LINES/2)+2 , (COLS - strlen(myname))/2 ,"%s", myname);
    attroff( COLOR_PAIR(1));
    refresh();
    sleep(2);
    clear();
}
void start_menu() {
    char *choices[] = {
        "1. Create New Rogue",
        "2. Rogue Login",
        "3. Before You Play",
        "4. Exit",
        NULL,
    };
    ITEM **items;
    MENU *menu;
    WINDOW *menu_win;
    int n_choices, c, i;

        // Count the number of choices
    n_choices = ARRAY_SIZE(choices) - 1;

    // Create items
    items = (ITEM **)calloc(n_choices + 1, sizeof(ITEM *));
    for (i = 0; i < n_choices; ++i)
        items[i] = new_item(choices[i], NULL);
    items[n_choices] = NULL;

    // Create menu
    menu = new_menu((ITEM **)items);

    // Create menu window
    int win_width = 40, win_height = 10;
    int startx = (COLS - win_width) / 2;
    int starty = (LINES - win_height) / 2;
    menu_win = newwin(win_height, win_width, starty, startx);
    keypad(menu_win, TRUE);

    // Set menu window and subwindow
    set_menu_win(menu, menu_win);
    set_menu_sub(menu, derwin(menu_win, win_height - 4, win_width - 2, 3, 1));

    // Set menu mark
    set_menu_mark(menu, " * ");

    // Print a border and title
    box(menu_win, 0, 0);
    print_in_middle(menu_win, 1, 0, win_width, "Game Menu", COLOR_PAIR(1));
    mvwaddch(menu_win, 2, 0, ACS_LTEE);
    mvwhline(menu_win, 2, 1, ACS_HLINE, win_width - 2);
    mvwaddch(menu_win, 2, win_width - 1, ACS_RTEE);

    // Post the menu
    post_menu(menu);
    wrefresh(menu_win);

    // Menu interaction loop
    while ((c = wgetch(menu_win)) != 'q' && successfully_logged_in == false) {
        switch (c) {
            case KEY_DOWN:
                menu_driver(menu, REQ_DOWN_ITEM);
                break;
            case KEY_UP:
                menu_driver(menu, REQ_UP_ITEM);
                break;
            case 10:  // Enter key
            {
                ITEM *cur = current_item(menu);
                if (strcmp(item_name(cur), "4. Exit") == 0) {
                    // Exit the menu loop
                    unpost_menu(menu);
                    free_menu(menu);
                    for (i = 0; i < n_choices; ++i)
                        free_item(items[i]);
                    // endwin();

                } else if (strcmp(item_name(cur), "1. Create New Rogue") == 0) {
                    create_new_rogue(menu, menu_win);
                } else if (strcmp(item_name(cur), "2. Rogue Login") == 0) {
                    login_rogue(menu, menu_win);
                } else if (strcmp(item_name(cur), "3. Before You Play") == 0) {
                    before_you_play(menu, menu_win);
                }

            }
                break;

        }
        if (successfully_logged_in == false) {
            box(menu_win, 0, 0);
            print_in_middle(menu_win, 1, 0, win_width, "Game Menu", COLOR_PAIR(1));
            mvwaddch(menu_win, 2, 0, ACS_LTEE);
            mvwhline(menu_win, 2, 1, ACS_HLINE, win_width - 2);
            mvwaddch(menu_win, 2, win_width - 1, ACS_RTEE);

            // Post the menu
            post_menu(menu);
            wrefresh(menu_win);
        }
    }
    if (successfully_logged_in == true) {
        unpost_menu(menu);
        free_menu(menu);
        for (i = 0; i < n_choices; ++i)
            free_item(items[i]);

    }
}
void start_map(Room room[]) {
    clear();
    generate_rooms(room);

    // Draw all rooms and connect them
    for (int i = 0; i < MAX_ROOMS; ++i) {
        draw_room(&room[i], i);
    }
    connect_rooms(&room[0], &room[1]);
    connect_rooms(&room[0], &room[2]);
    connect_rooms(&room[1], &room[3]);
    connect_rooms(&room[2], &room[3]);
    connect_rooms(&room[2], &room[4]);
    connect_rooms(&room[4], &room[5]);
    connect_rooms(&room[3], &room[5]);

    refresh();
}
void start_player_position(Pos *player,Room room[]) {
    int which_room = rand() % 6;
    // int x = rand() % (room[which_room].width + 1);
    // int y = rand() % (room[which_room].height + 1);

    player->x = 3 + room->x;
    player->y = 3 + room->y;
}

void handle_movement(int ch, Pos *player, char map[LINES][COLS]){

    if (player->y > 0 && player->y < LINES - 1 && player->x > 0 && player->x < COLS - 1) {

        if (ch == 'k' || ch == 'K' || ch == '8' || ch == KEY_UP) {      //move up
            if (is_corridor(map, *player) && map[player->y - 1][player->x] == '#' && map[player->y - 1][player->x] == '+')
                player->y -- ;
            if  (is_room(map, *player) && map[player->y - 1][player->x] != '_' && map[player->y - 1][player->x] != '|')
                player->y -- ;

        }
        else if (ch == 'j' || ch == 'J' || ch == '2' || ch == KEY_DOWN) {   //move down
            if ((is_corridor(map, *player) && map[player->y + 1][player->x] == '#' && map[player->y + 1][player->x] == '+') || (is_room(map, *player) && map[player->y + 1][player->x] != '_' && map[player->y + 1][player->x] != '|') )
                player->y ++ ;
        }
        else if (ch == 'h' || ch == 'H' || ch == '4' || ch == KEY_LEFT) {
            //move left
            if ((is_corridor(map, *player) && map[player->y][player->x - 1] == '#' && map[player->y][player->x - 1] == '+') || (is_room(map, *player) && map[player->y][player->x - 1] != '_' && map[player->y][player->x - 1] != '|') )
                player->x -- ;
        }
        else if (ch == 'l' || ch == 'L' || ch == '6' || ch == KEY_RIGHT) {   //move right
                if ((is_corridor(map, *player) && map[player->y][player->x + 1] == '#' && map[player->y][player->x + 1] == '+') || (is_room(map, *player) && map[player->y][player->x + 1] != '_' && map[player->y][player->x + 1] != '|') )
                    player->x ++ ;
        }
        // else if (ch == 'u' || ch == 'U' ) {   //اریب بالا راست
        //     if (){
        //         player->y -- ;
        //         player->x ++ ;
        //     }
        // }
        // else if (ch == 'y' || ch == 'Y' ) {   //اریب بالا چپ
        //     if (){
        //         player->y -- ;
        //         player->x -- ;
        //     }
        // }
        // else if (ch == 'n' || ch == 'N' ) {   //اریب پایین راست
        //     if (){
        //         player->y ++ ;
        //         player->x ++ ;
        //     }
        // }
        // else if (ch == 'b' || ch == 'B' ) {   //اریب پایین چپ
        //     if () {
        //         player->y ++ ;
        //         player->x -- ;
        //     }
        // }
        else if (ch == 'q' || ch == 'Q') {
            endwin();
            exit(0);
        }
        else {
            mvprintw(0, 0, "Invalid Command");
            refresh();
        }

    }



}

bool is_corridor(char map[LINES][COLS],Pos player) {
    if (map[player.y][player.x] == '#' || map[player.y][player.x] == '+') {
        return true;
    }
    return false;

}


bool is_room(char map[LINES][COLS], Pos player) {
    if (map[player.y][player.x] == '.' || map[player.y][player.x] == '+') {
        return true;
    }
    return false;

}



void save_screen_to_array(char map[LINES][COLS]) {
    for (int y = 0; y < LINES; ++y) {
        for (int x = 0; x < COLS; ++x) {
            // Get the character at the screen position and store it
            map[y][x] = mvinch(y, x) & A_CHARTEXT;
        }
    }
}
void print_map_from_array(char map[LINES][COLS]) {
    for (int y = 0; y < LINES; ++y) {
        for (int x = 0; x < COLS; ++x) {
            mvaddch(y, x, map[y][x]); // Print each character back to the screen
        }
    }
}
void save_map_to_file(const char *filename, char map[LINES][COLS]) {
    FILE *file = fopen(filename, "w"); // Open file in write mode
    if (file == NULL) {
        perror("Error opening file for saving");
        return;
    }

    for (int y = 0; y < LINES; ++y) {
        for (int x = 0; x < COLS; ++x) {
            fputc(map[y][x], file); // Write each character
        }
        fputc('\n', file); // Add a newline at the end of each row
    }

    fclose(file); // Close the file
}
void load_map_from_file(const char *filename, char map[LINES][COLS]) {
    FILE *file = fopen(filename, "r"); // Open file in read mode
    if (file == NULL) {
        perror("Error opening file for loading");
        return;
    }

    for (int y = 0; y < LINES; ++y) {
        for (int x = 0; x < COLS; ++x) {
            int ch = fgetc(file); // Read each character
            if (ch == EOF || ch == '\n') break;
            map[y][x] = ch;
        }
        // Skip any remaining characters on the line or newline
        while (fgetc(file) != '\n' && !feof(file));
    }

    fclose(file); // Close the file
}