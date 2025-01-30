#include "essentials.h"
#include "game_menu.h"
#include "map.h"


void start_ncurses();
void start_colors();
void start_game();
void start_map(Room room[]);
void start_player_position(Room room[]);
void handle_movement(int ch,  char map[MAX_ROW][COLS]);
bool is_corridor(char map[MAX_ROW][COLS]);
bool is_room(char map[MAX_ROW][COLS]);
void draw_player();

void save_screen_to_array(char map[MAX_ROW][COLS]);
void print_map_from_array(char map[MAX_ROW][COLS]) ;
void save_map_to_file(const char *filename, char map[MAX_ROW][COLS]);
void load_map_from_file(const char *filename, char map[MAX_ROW][COLS]);


int main() {


    start_ncurses();
    start_game();
    start_menu();

    curs_set(FALSE);
    Room room[MAX_ROOMS];
    start_map(room);
    char map[MAX_ROW][COLS];
    save_screen_to_array(map);
    save_map_to_file("map.txt", map);
    start_player_position(room);

    while (1) {
        //DRAW MAP
        print_map_from_array(map);
        mvprintw(0, 0, "Health: 100 | Level: 1 | Press 'Q' to quit");
        draw_player();

        //MOVE PLAYER
        int ch = getch();
        if (ch == 'q' || ch == 'Q') {
            break;
        }
        handle_movement(ch, map);
    }

    endwin();
    exit(0);

}

void start_ncurses() {
    // setlocale(LC_ALL, "");

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    start_colors();
    srand(time(NULL));
    clear();
    refresh();
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
    attron(A_BOLD |  COLOR_PAIR(3));
    // char message[] = "ROGUE";
    // mvprintw((LINES / 2)-1, (COLS - strlen(message)) / 2, "%s", message); // نمایش پیام در مرکز
    // attroff(A_BOLD);
    //
    char myname[] = "Fatemeh Nilforoushan";
    mvprintw((LINES/2)+5 , (COLS - strlen(myname) -3)/2 ,"%s", myname);

    mvprintw(LINES/2 - 5 , (COLS - 30)/2, " (       )                   ");
    mvprintw(LINES/2 - 4, (COLS - 30)/2, " )\\ ) ( /( (                 ");
    mvprintw(LINES/2 - 3, (COLS - 30)/2, "(()/( )\\()))\\ )      (  (    ");
    mvprintw(LINES/2 - 2, (COLS - 30)/2, " /(_)|(_)\\(()/(      )\\ )\\   ");
    mvprintw(LINES/2 - 1, (COLS - 30)/2, "(_))   ((_)/(_))_ _ ((_|(_)  ");
    mvprintw(LINES/2  , (COLS - 30)/2, "| _ \\ / _ (_)) __| | | | __| ");
    mvprintw(LINES/2 + 1, (COLS - 30)/2, "|   /| (_) || (_ | |_| | _|  ");
    mvprintw(LINES/2 + 2, (COLS - 30)/2, "|_|_\\ \\___/  \\___|\\___/|___| ");

    // mvprintw(LINES/2    , COLS/2, "     ___           ___           ___           ___           ___     ");
    // mvprintw(LINES/2 + 1, COLS/2, "    /\\  \\         /\\  \\         /\\  \\         /\\__\\         /\\  \\   ");
    // mvprintw(LINES/2 + 2, COLS/2, "   /::\\  \\       /::\\  \\       /::\\  \\       /:/  /        /::\\  \\   ");
    // mvprintw(LINES/2 + 3, COLS/2, "  /:/\\:\\  \\     /:/\\:\\  \\     /:/\\:\\  \\     /:/  /        /:/\\:\\  \\  ");
    // mvprintw(LINES/2 + 4, COLS/2, " /::\\~\\:\\  \\   /:/  \\:\\  \\   /:/  \\:\\  \\   /:/  /  ___   /::\\~\\:\\  \\ ");
    // mvprintw(LINES/2 + 5, COLS/2, "/:/\\:\\ \\:\\__\\ /:/__/ \\:\\__\\ /:/__/_\\:\\__\\ /:/__/  /\\__\\ /:/\\:\\ \\:\\__\\");
    // mvprintw(LINES/2 + 6, COLS/2, "\\/_|::\\/:/  / \\:\\  \\ /:/  / \\:\\  /\\ \\/__/ \\:\\  \\ /:/  / \\:\\~\\:\\ \\/__/");
    // mvprintw(LINES/2 + 7, COLS/2, "   |:|::/  /   \\:\\  /:/  /   \\:\\ \\:\\__\\    \\:\\  /:/  /   \\:\\ \\:\\__\\  ");
    // mvprintw(LINES/2 + 8, COLS/2, "   |:|\\/__/     \\:\\/:/  /     \\:\\/:/  /     \\:\\/:/  /     \\:\\ \\/__/  ");
    // mvprintw(LINES/2 + 9, COLS/2, "   |:|  |        \\::/  /       \\::/  /       \\::/  /       \\:\\__\\    ");
    // mvprintw(LINES/2 + 10, COLS/2, "    \\|__|         \\/__/         \\/__/         \\/__/         \\/__/    ");


    attroff( A_BOLD | COLOR_PAIR(1));
    refresh();
    sleep(1);
    clear();
    refresh();

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

    // print bottom line of the page
    const char *bottom_line = "_ . ";
    int pattern_length = 4;
    for (int i = 0; i < COLS; i += pattern_length) {
        mvprintw(LINES - 6, i, bottom_line);
    }

    refresh();
}
void start_player_position(Room room[]) {
    int which_room = rand() % 6;
    // int x = rand() % (room[which_room].width + 1);
    // int y = rand() % (room[which_room].height + 1);

    // player->x = 3 + room[which_room].x;
    // player->y = 3 + room[which_room].y;

    player.player_pos.x = 3 + room[which_room].x;
    player.player_pos.y = 3 + room[which_room].y;
}
// void handle_movement(int ch, Pos *player, char map[LINES][COLS]){
// int do_nothing = 0;
//     if (player->y >= 0 && player->y <= LINES - 1 && player->x >= 0 && player->x <= COLS - 1) {
//
//         if (ch == 'k' || ch == 'K' || ch == '8' || ch == KEY_UP) {      //move up
//             if (map[player->y - 1][player->x] == ' ')
//                 do_nothing = 1;
//             else if ((is_corridor(map, *player) && (map[player->y - 1][player->x] == '#' || map[player->y - 1][player->x] == '+')) ||
//                 (is_room(map, *player) && map[player->y - 1][player->x] != '_' && map[player->y - 1][player->x] != '|') )
//                 player->y -- ;
//         }
//         else if (ch == 'j' || ch == 'J' || ch == '2' || ch == KEY_DOWN) {   //move down
//             if (map[player->y + 1][player->x] == ' ')
//                 do_nothing = 1;
//             else if ((is_corridor(map, *player) && (map[player->y + 1][player->x] == '#' || map[player->y + 1][player->x] == '+')) ||
//                 (is_room(map, *player) && map[player->y + 1][player->x] != '_' && map[player->y + 1][player->x] != '|') )
//                 player->y ++ ;
//         }
//         else if (ch == 'h' || ch == 'H' || ch == '4' || ch == KEY_LEFT) {     //move left
//             if (map[player->y][player->x - 1] == ' ')
//                 do_nothing = 1;
//             else if ((is_corridor(map, *player) && (map[player->y][player->x - 1] == '#' || map[player->y][player->x - 1] == '+')) ||
//                 (is_room(map, *player) && map[player->y][player->x - 1] != '_' && map[player->y][player->x - 1] != '|') )
//                 player->x -- ;
//         }
//         else if (ch == 'l' || ch == 'L' || ch == '6' || ch == KEY_RIGHT) {   //move right
//             if (map[player->y][player->x + 1] == ' ')
//                 do_nothing = 1;
//             else if ((is_corridor(map, *player) && (map[player->y][player->x + 1] == '#' || map[player->y][player->x + 1] == '+')) ||
//                     (is_room(map, *player) && map[player->y][player->x + 1] != '_' && map[player->y][player->x + 1] != '|') )
//                     player->x ++ ;
//
//         }
//         else if (ch == 'u' || ch == 'U' ) {   //اریب بالا راست
//             if (map[player->y -1][player->x +1] == ' ' || map[player->y -1][player->x +1] == '|' || map[player->y -1][player->x +1] == '_') {
//                 do_nothing = 1;
//             }
//             else if ( ((is_corridor(map, *player) && (map[player->y - 1][player->x] == '#' || map[player->y - 1][player->x] == '+')) ||
//                 (is_room(map, *player) && map[player->y - 1][player->x] != '_' && map[player->y - 1][player->x] != '|')) &&
//                 ((is_corridor(map, *player) && (map[player->y][player->x + 1] == '#' || map[player->y][player->x + 1] == '+')) ||
//                     (is_room(map, *player) && map[player->y][player->x + 1] != '_' && map[player->y][player->x + 1] != '|') ))
//                 {
//                 player->y -- ;
//                 player->x ++ ;
//             }
//         }
//         else if (ch == 'y' || ch == 'Y' ) {   //اریب بالا چپ
//             if (map[player->y -1][player->x -1] == ' ' || map[player->y -1][player->x -1] == '|' || map[player->y -1][player->x -1] == '_') {
//                 do_nothing = 1;
//             }
//             else if ( ((is_corridor(map, *player) && (map[player->y - 1][player->x] == '#' || map[player->y - 1][player->x] == '+')) ||
//                 (is_room(map, *player) && map[player->y - 1][player->x] != '_' && map[player->y - 1][player->x] != '|')) &&
//                 ((is_corridor(map, *player) && (map[player->y][player->x - 1] == '#' || map[player->y][player->x - 1] == '+')) ||
//                 (is_room(map, *player) && map[player->y][player->x - 1] != '_' && map[player->y][player->x - 1] != '|') ) )
//                 {
//                 player->y -- ;
//                 player->x -- ;
//             }
//         }
//         else if (ch == 'n' || ch == 'N' ) {   //اریب پایین راست
//             if (map[player->y +1][player->x +1] == ' ' || map[player->y +1][player->x +1] == '|' || map[player->y +1][player->x +1] == '_') {
//                 do_nothing = 1;
//             }
//             else if (((is_corridor(map, *player) && (map[player->y + 1][player->x] == '#' || map[player->y + 1][player->x] == '+')) ||
//                 (is_room(map, *player) && map[player->y + 1][player->x] != '_' && map[player->y + 1][player->x] != '|') )      &&
//             ((is_corridor(map, *player) && (map[player->y][player->x + 1] == '#' || map[player->y][player->x + 1] == '+')) ||
//                 (is_room(map, *player) && map[player->y][player->x + 1] != '_' && map[player->y][player->x + 1] != '|') ))
//                 {
//                 player->y ++ ;
//                 player->x ++ ;
//             }
//         }
//         else if (ch == 'b' || ch == 'B' ) {   //اریب پایین چپ
//             if (map[player->y +1][player->x -1] == ' ' || map[player->y +1][player->x -1] == '|' || map[player->y +1][player->x -1] == '_') {
//                 do_nothing = 1;
//             }
//             else if (((is_corridor(map, *player) && (map[player->y + 1][player->x] == '#' || map[player->y + 1][player->x] == '+')) ||
//                 (is_room(map, *player) && map[player->y + 1][player->x] != '_' && map[player->y + 1][player->x] != '|') )     &&
//                 ((is_corridor(map, *player) && (map[player->y][player->x - 1] == '#' || map[player->y][player->x - 1] == '+')) ||
//                 (is_room(map, *player) && map[player->y][player->x - 1] != '_' && map[player->y][player->x - 1] != '|') ))
//                 {
//                 player->y ++ ;
//                 player->x -- ;
//             }
//         }
//
//         else {
//             mvprintw(0, 0, "Invalid Command");
//             refresh();
//             sleep(0.5);
//         }
//
//     }
//
//
//
// }
void handle_movement(int ch, char map[MAX_ROW][COLS]){
    int do_nothing = 0;
    if (player.player_pos.y >= 0 && player.player_pos.y <= LINES - 1 && player.player_pos.x >= 0 && player.player_pos.x <= COLS - 1) {

        if (ch == 'k' || ch == 'K' || ch == '8' || ch == KEY_UP) {      //move up
            if (map[player.player_pos.y - 1][player.player_pos.x] == ' ')
                do_nothing = 1;
            else if ((is_corridor(map) && (map[player.player_pos.y - 1][player.player_pos.x] == '#' || map[player.player_pos.y - 1][player.player_pos.x] == '+')) ||
                (is_room(map) && map[player.player_pos.y - 1][player.player_pos.x] != '_' && map[player.player_pos.y - 1][player.player_pos.x] != '|') )
                player.player_pos.y -- ;
        }
        else if (ch == 'j' || ch == 'J' || ch == '2' || ch == KEY_DOWN) {   //move down
            if (map[player.player_pos.y + 1][player.player_pos.x] == ' ')
                do_nothing = 1;
            else if ((is_corridor(map) && (map[player.player_pos.y + 1][player.player_pos.x] == '#' || map[player.player_pos.y + 1][player.player_pos.x] == '+')) ||
                (is_room(map) && map[player.player_pos.y + 1][player.player_pos.x] != '_' && map[player.player_pos.y + 1][player.player_pos.x] != '|') )
                player.player_pos.y ++ ;
        }
        else if (ch == 'h' || ch == 'H' || ch == '4' || ch == KEY_LEFT) {     //move left
            if (map[player.player_pos.y][player.player_pos.x - 1] == ' ')
                do_nothing = 1;
            else if ((is_corridor(map) && (map[player.player_pos.y][player.player_pos.x - 1] == '#' || map[player.player_pos.y][player.player_pos.x - 1] == '+')) ||
                (is_room(map) && map[player.player_pos.y][player.player_pos.x - 1] != '_' && map[player.player_pos.y][player.player_pos.x - 1] != '|') )
                player.player_pos.x -- ;
        }
        else if (ch == 'l' || ch == 'L' || ch == '6' || ch == KEY_RIGHT) {   //move right
            if (map[player.player_pos.y][player.player_pos.x + 1] == ' ')
                do_nothing = 1;
            else if ((is_corridor(map) && (map[player.player_pos.y][player.player_pos.x + 1] == '#' || map[player.player_pos.y][player.player_pos.x + 1] == '+')) ||
                    (is_room(map) && map[player.player_pos.y][player.player_pos.x + 1] != '_' && map[player.player_pos.y][player.player_pos.x + 1] != '|') )
                    player.player_pos.x ++ ;

        }
        else if (ch == 'u' || ch == 'U' ) {   //اریب بالا راست
            if (map[player.player_pos.y -1][player.player_pos.x +1] == ' ' || map[player.player_pos.y -1][player.player_pos.x +1] == '|' || map[player.player_pos.y -1][player.player_pos.x +1] == '_') {
                do_nothing = 1;
            }
            else if ( ((is_corridor(map) && (map[player.player_pos.y - 1][player.player_pos.x] == '#' || map[player.player_pos.y - 1][player.player_pos.x] == '+')) ||
                (is_room(map) && map[player.player_pos.y - 1][player.player_pos.x] != '_' && map[player.player_pos.y - 1][player.player_pos.x] != '|')) &&
                ((is_corridor(map) && (map[player.player_pos.y][player.player_pos.x + 1] == '#' || map[player.player_pos.y][player.player_pos.x + 1] == '+')) ||
                    (is_room(map) && map[player.player_pos.y][player.player_pos.x + 1] != '_' && map[player.player_pos.y][player.player_pos.x + 1] != '|') ))
                {
                player.player_pos.y -- ;
                player.player_pos.x ++ ;
            }
        }
        else if (ch == 'y' || ch == 'Y' ) {   //اریب بالا چپ
            if (map[player.player_pos.y -1][player.player_pos.x -1] == ' ' || map[player.player_pos.y -1][player.player_pos.x -1] == '|' || map[player.player_pos.y -1][player.player_pos.x -1] == '_') {
                do_nothing = 1;
            }
            else if ( ((is_corridor(map) && (map[player.player_pos.y - 1][player.player_pos.x] == '#' || map[player.player_pos.y - 1][player.player_pos.x] == '+')) ||
                (is_room(map) && map[player.player_pos.y - 1][player.player_pos.x] != '_' && map[player.player_pos.y - 1][player.player_pos.x] != '|')) &&
                ((is_corridor(map) && (map[player.player_pos.y][player.player_pos.x - 1] == '#' || map[player.player_pos.y][player.player_pos.x - 1] == '+')) ||
                (is_room(map) && map[player.player_pos.y][player.player_pos.x - 1] != '_' && map[player.player_pos.y][player.player_pos.x - 1] != '|') ) )
                {
                player.player_pos.y -- ;
                player.player_pos.x -- ;
            }
        }
        else if (ch == 'n' || ch == 'N' ) {   //اریب پایین راست
            if (map[player.player_pos.y +1][player.player_pos.x +1] == ' ' || map[player.player_pos.y +1][player.player_pos.x +1] == '|' || map[player.player_pos.y +1][player.player_pos.x +1] == '_') {
                do_nothing = 1;
            }
            else if (((is_corridor(map) && (map[player.player_pos.y + 1][player.player_pos.x] == '#' || map[player.player_pos.y + 1][player.player_pos.x] == '+')) ||
                (is_room(map) && map[player.player_pos.y + 1][player.player_pos.x] != '_' && map[player.player_pos.y + 1][player.player_pos.x] != '|') )      &&
            ((is_corridor(map) && (map[player.player_pos.y][player.player_pos.x + 1] == '#' || map[player.player_pos.y][player.player_pos.x + 1] == '+')) ||
                (is_room(map) && map[player.player_pos.y][player.player_pos.x + 1] != '_' && map[player.player_pos.y][player.player_pos.x + 1] != '|') ))
                {
                player.player_pos.y ++ ;
                player.player_pos.x ++ ;
            }
        }
        else if (ch == 'b' || ch == 'B' ) {   //اریب پایین چپ
            if (map[player.player_pos.y +1][player.player_pos.x -1] == ' ' || map[player.player_pos.y +1][player.player_pos.x -1] == '|' || map[player.player_pos.y +1][player.player_pos.x -1] == '_') {
                do_nothing = 1;
            }
            else if (((is_corridor(map) && (map[player.player_pos.y + 1][player.player_pos.x] == '#' || map[player.player_pos.y + 1][player.player_pos.x] == '+')) ||
                (is_room(map) && map[player.player_pos.y + 1][player.player_pos.x] != '_' && map[player.player_pos.y + 1][player.player_pos.x] != '|') )     &&
                ((is_corridor(map) && (map[player.player_pos.y][player.player_pos.x - 1] == '#' || map[player.player_pos.y][player.player_pos.x - 1] == '+')) ||
                (is_room(map) && map[player.player_pos.y][player.player_pos.x - 1] != '_' && map[player.player_pos.y][player.player_pos.x - 1] != '|') ))
                {
                player.player_pos.y ++ ;
                player.player_pos.x -- ;
            }
        }

        else {
            mvprintw(0, 0, "Invalid Command");
            refresh();
            sleep(0.5);
        }

    }
}

bool is_corridor(char map[MAX_ROW][COLS]) {
    if (map[player.player_pos.y][player.player_pos.x] == '#' || map[player.player_pos.y][player.player_pos.x] == '+') {
        return true;
    }
    return false;

}
bool is_room(char map[MAX_ROW][COLS]) {
    if (map[player.player_pos.y][player.player_pos.x] == '.' || map[player.player_pos.y][player.player_pos.x] == '+') {
        return true;
    }
    return false;

}

void draw_player() {
    //attron
    switch (player.player_color) {
        case 1: {
            attron(COLOR_PAIR(1));  //Magenta
            break;
        }
        case 3: {
            attron(COLOR_PAIR(3));  //Red
            break;
        }
        case 4: {
            attron(COLOR_PAIR(4));  //Green
            break;
        }
        case 5: {
            attron(COLOR_PAIR(5)); // Blue
            break;
        }
        case 7: {
            attron(COLOR_PAIR(7)); //Yellow
            break;
        }
        default:
            attron(COLOR_PAIR(3));
    }
    noecho();
    // mvaddwstr(player.y, player.x, L"🧑");
    // mvaddwstr(player.y, player.x, L"웃");
    mvaddch(player.player_pos.y, player.player_pos.x, '@');
    //attroff
    switch (player.player_color) {
        case 1: {
            attroff(COLOR_PAIR(1));  //Magenta
            break;
        }
        case 3: {
            attroff(COLOR_PAIR(3));  //Red
            break;
        }
        case 4: {
            attroff(COLOR_PAIR(4));  //Green
            break;
        }
        case 5: {
            attroff(COLOR_PAIR(5)); // Blue
            break;
        }
        case 7: {
            attroff(COLOR_PAIR(7)); //Yellow
            break;
        }
        default:
            attroff(COLOR_PAIR(3));
    }
}

void save_screen_to_array(char map[MAX_ROW][COLS]) {
    for (int y = 0; y < MAX_ROW; ++y) {
        for (int x = 0; x < COLS; ++x) {
            // Get the character at the screen position and store it
            map[y][x] = mvinch(y, x) & A_CHARTEXT;
        }
    }
}
void print_map_from_array(char map[MAX_ROW][COLS]) {
    for (int y = 0; y < MAX_ROW; ++y) {
        for (int x = 0; x < COLS; ++x) {
            mvaddch(y, x, map[y][x]); // Print each character back to the screen
        }
    }
}
void save_map_to_file(const char *filename, char map[MAX_ROW][COLS]) {
    FILE *file = fopen(filename, "w"); // Open file in write mode
    if (file == NULL) {
        perror("Error opening file for saving");
        return;
    }

    for (int y = 0; y < MAX_ROW; ++y) {
        for (int x = 0; x < COLS; ++x) {
            fputc(map[y][x], file); // Write each character
        }
        fputc('\n', file); // Add a newline at the end of each row
    }

    fclose(file); // Close the file
}
void load_map_from_file(const char *filename, char map[MAX_ROW][COLS]) {
    FILE *file = fopen(filename, "r"); // Open file in read mode
    if (file == NULL) {
        perror("Error opening file for loading");
        return;
    }

    for (int y = 0; y < MAX_ROW; ++y) {
        for (int x = 0; x < COLS; ++x) {
            int ch = fgetc(file); // Read each character
            if (ch == EOF || ch == '\n') break;
            map[y][x] = (char)ch;
        }
        // Skip any remaining characters on the line or newline
        while (fgetc(file) != '\n' && !feof(file));
    }

    fclose(file); // Close the file
}