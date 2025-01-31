#include <bits/local_lim.h>

#include "essentials.h"
#include "game_menu.h"
#include "map.h"

//initializing game
void start_ncurses();
void start_colors();
void start_window();
void init_map(Room room[MAX_ROOMS]);
void start_map(Room room[MAX_ROOMS],  char map[MAX_ROW][COLS]);
void start_player_position(Room room[MAX_ROOMS]);
void draw_player();
void start_music();
//movement
void handle_movement(int ch,  char map[MAX_ROW][COLS]);
bool is_corridor(char map[MAX_ROW][COLS]);
bool is_room(char map[MAX_ROW][COLS]);
//massaging
void bottom_massage();
void clear_top_massage();
//next floor
void on_stair(int ch, char map[MAX_ROW][COLS], Room room[MAX_ROOMS]) ;
void next_floor();
//visibility
void init_visibility(bool visibility[MAX_ROW][MAX_COL] );
void reveal_room(Room *room, bool visibility[MAX_ROW][MAX_COL] );
void reveal_corridor(int player_x, int player_y, int direction, bool visibility[MAX_ROW][MAX_COL] ) ;

int main() {


    start_ncurses();
    start_window();
    start_music();
    start_menu();
    curs_set(FALSE);
    Room room[MAX_ROOMS];
    char map[MAX_ROW][COLS];
    bool visibility[MAX_ROW][MAX_COL] ;
    init_visibility(visibility);
    start_map(room, map);
    start_player_position(room);
    reveal_room(&room[player.first_room_pos], visibility );
    player.a_pressed = false;
    while (1) {
        //DRAW MAP
        print_map_from_array(map, visibility);
        draw_player();
        bottom_massage();
        //MOVE PLAYER
        int ch = getch();
        if (ch == 'q' || ch == 'Q')   break;
        on_stair(ch, map, room);
        handle_movement(ch, map);

        // clear_top_massage();

    }

    endwin();
    exit(0);

}

//initializing game
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
    init_pair(9, COLOR_WHITE,COLOR_WHITE);
}
void start_window() {
    //show my name
    attron(A_BOLD |  COLOR_PAIR(3));
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
void start_music() {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        endwin();
        exit(0);
        return ;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer could not initialize! Mix Error: %s\n", Mix_GetError());
        endwin();
        exit(0);
        return ;
    }
    play_music("sneaky_snitch.mp3");

}
void init_map(Room room[MAX_ROOMS]) {
    clear();
    generate_rooms(room);
    // Draw all rooms and connect them
    for (int i = 0; i < MAX_ROOMS; ++i) {
        draw_room(&room[i], i);
        room->visited = false;
    }
    connect_rooms(&room[0], &room[1]);
    connect_rooms(&room[0], &room[2]);
    connect_rooms(&room[1], &room[3]);
    connect_rooms(&room[2], &room[3]);
    connect_rooms(&room[2], &room[4]);
    connect_rooms(&room[4], &room[5]);
    connect_rooms(&room[3], &room[5]);

}
void start_map(Room room[MAX_ROOMS], char map[MAX_ROW][COLS]) {
    if ((player.guest || player.creat_game_bool) && !player.resume_game_bool) {//start a new game, its a guest or new player
        init_map(room);

        //draw stair for first floor
        player.stair_pos = rand() % 6;
        if (player.stair_pos == player.first_room_pos) player.stair_pos = 3;
        draw_forward_stair(&room[player.stair_pos]);
        bottom_massage();   //for saving level
        save_screen_to_array(map);
        // draw_window(0, 0, room, map);
        // draw_window(4,0, room, map);
        // draw_window(3, 0, room, map);
        save_map_to_file("map.txt", map);

    }else if (player.resume_game_bool && !player.creat_game_bool && !player.guest) {//continue last game, it has logged in before.
        load_map_from_file("map.txt", map);
        detect_rooms(map, room);

    }
}
void start_player_position(Room room[MAX_ROOMS]) {
    int which_room = rand() % 6;
    player.player_pos.x = 2 + room[which_room].x;
    player.player_pos.y = 2 + room[which_room].y;
    player.first_room_pos = which_room;
}
void draw_player() {
    attron(A_BOLD);
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
    attroff(A_BOLD);
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

//massaging
void bottom_massage() {
    attron(A_BOLD | COLOR_PAIR(8) );  //gold
    mvprintw(LINES - 2, 0, "\tLevel : %d\tHealth : %d\tGold : %d\tArmor : %d", player.floor, player.health, player.gold, 5);
    attroff( A_BOLD | COLOR_PAIR(8));
    refresh();
}
void clear_top_massage() {
    for (int i=0; i<100; i++) {
        mvaddch(0, i, ' ');
    }
}

//next floor
void on_stair(int ch, char map[MAX_ROW][COLS], Room room[MAX_ROOMS]) {
    if (map[player.player_pos.y][player.player_pos.x] == '<') {  //standing on a forward stair
        if (ch == 'a' || ch == 'A') { // Detect 'A' key
            player.a_pressed = true;         // Set flag when 'A' is detected
        } else if (player.a_pressed && ch == KEY_UP) {
            player.a_pressed = false; // Reset flag
            //make next floor
            next_floor(room , map);
        } else {
            player.a_pressed = false; // Reset flag if another key is pressed
        }
    }
}
void next_floor(Room room[MAX_ROOMS], char map[MAX_ROW][COLS]) {
    //make new map
    init_map(room);
    //update player floor
    player.floor ++;
    bottom_massage();   //for saving level

    //draw stair to last floor & player new position
    draw_backward_stair_player_pos(&room[player.stair_pos]);

    //draw random stair for next floor in different room
    if (player.floor < 4) {
        int stair_new_pos = rand() % 6;
        if (stair_new_pos == player.stair_pos && stair_new_pos != 5) {
            player.stair_pos = stair_new_pos+1;
        }else if (stair_new_pos == player.stair_pos && stair_new_pos == 5) {
            player.stair_pos = 0;
        }else {
            player.stair_pos = stair_new_pos;
        }
        draw_forward_stair(&room[player.stair_pos]);
    }


    save_screen_to_array(map);
    save_map_to_file("map.txt", map);


}

//movement
void handle_movement(int ch, char map[MAX_ROW][COLS]){
    int next_move;
    int do_nothing = 0;


    if (ch == 'k' || ch == 'K' || ch == '8' || ch == KEY_UP) {      //move up
        next_move = map[player.player_pos.y - 1][player.player_pos.x];
        if (next_move == '<')      mvprintw(0,0,"It's a Stair! Press A + Key_Up To Go To Next Floor.");

        if (next_move == ' ' || next_move == '|' || next_move == '_')
            return;   //do nothing
        if (  (is_corridor(map) && (next_move == '#' || next_move == '+')) || is_room(map) )
            player.player_pos.y -- ;
        }


    else if (ch == 'j' || ch == 'J' || ch == '2' || ch == KEY_DOWN) {   //move down
        next_move = map[player.player_pos.y + 1][player.player_pos.x];
        if (next_move == '<')      mvprintw(0,0,"It's a Stair! Press A + Key_Up To Go To Next Floor.");

        if ( next_move == ' ' || next_move == '|' || next_move == '_')
                return;   //do nothing
        if (  (is_corridor(map) && (next_move == '#' || next_move == '+')) || is_room(map) )
                player.player_pos.y ++ ;
        }

    else if (ch == 'h' || ch == 'H' || ch == '4' || ch == KEY_LEFT) {     //move left
        next_move = map[player.player_pos.y][player.player_pos.x - 1];
        if (next_move == '<')      mvprintw(0,0,"It's a Stair! Press A + Key_Up To Go To Next Floor.");

        if (next_move == ' ' || next_move == '|' || next_move == '_')
                return;   //do nothing
        if (  (is_corridor(map) && (next_move == '#' || next_move == '+')) || is_room(map) )
                player.player_pos.x -- ;
        }

    else if (ch == 'l' || ch == 'L' || ch == '6' || ch == KEY_RIGHT) {   //move right
        next_move = map[player.player_pos.y][player.player_pos.x + 1];
        if (next_move == '<')      mvprintw(0,0,"It's a Stair! Press A + Key_Up To Go To Next Floor.");

        if (next_move == ' ' || next_move == '|' || next_move == '_')
                return;   //do nothing
        if (  (is_corridor(map) && (next_move == '#' || next_move == '+')) || is_room(map) )
                    player.player_pos.x ++ ;

        }
    else if (ch == 'u' || ch == 'U' ) {   //اریب بالا راست
        next_move = map[player.player_pos.y -1][player.player_pos.x +1];
        if (next_move == '<')      mvprintw(0,0,"It's a Stair! Press A + Key_Up To Go To Next Floor.");

        if (next_move == ' ' || next_move == '|' || next_move == '_') {
                return;   //do nothing
            }
        if (  (is_corridor(map) && (next_move == '#' || next_move == '+')) || is_room(map) ){
                player.player_pos.y -- ;
                player.player_pos.x ++ ;
            }
        }
    else if (ch == 'y' || ch == 'Y' ) {   //اریب بالا چپ
        next_move = map[player.player_pos.y -1][player.player_pos.x -1];
        if (next_move == '<')      mvprintw(0,0,"It's a Stair! Press A + Key_Up To Go To Next Floor.");

        if (next_move == ' ' || next_move == '|' || next_move == '_') {
                return;   //do nothing
            }
        if (  (is_corridor(map) && (next_move == '#' || next_move == '+')) || is_room(map) ){
                player.player_pos.y -- ;
                player.player_pos.x -- ;
            }
        }
    else if (ch == 'n' || ch == 'N' ) {   //اریب پایین راست
        next_move = map[player.player_pos.y +1][player.player_pos.x +1];
        if (next_move == '<')      mvprintw(0,0,"It's a Stair! Press A + Key_Up To Go To Next Floor.");

        if (next_move == ' ' || next_move == '|' || next_move == '_') {
                return;   //do nothing
            }
        if (  (is_corridor(map) && (next_move == '#' || next_move == '+')) || is_room(map) ) {
                player.player_pos.y ++ ;
                player.player_pos.x ++ ;
            }
        }
    else if (ch == 'b' || ch == 'B' ) {   //اریب پایین چپ
        next_move = map[player.player_pos.y +1][player.player_pos.x -1];
        if (next_move == '<')      mvprintw(0,0,"It's a Stair! Press A + Key_Up To Go To Next Floor.");

        if (next_move == ' ' || next_move == '|' || next_move == '_') {
                return;   //do nothing
            }
        if (  (is_corridor(map) && (next_move == '#' || next_move == '+')) || is_room(map) ){
                player.player_pos.y ++ ;
                player.player_pos.x -- ;
            }
        }
    else if (ch == 'a' || ch == 'A') {
        return;
    }

    else {
            mvprintw(0, 0, "Invalid Command");
            refresh();
            sleep(0.5);
        }


}
bool is_corridor(char map[MAX_ROW][COLS]) {
    if (map[player.player_pos.y][player.player_pos.x] == '#' || map[player.player_pos.y][player.player_pos.x] == '+') {
        return true;
    }
    return false;

}
bool is_room(char map[MAX_ROW][COLS]) {
    int pos = map[player.player_pos.y][player.player_pos.x];
    if (pos  == '.' || pos == '+' || pos == '<' || pos == '>') {
        return true;
    }
    return false;

}


//visibility
void init_visibility(bool visibility[MAX_ROW][MAX_COL] ) {
    for (int i = 0; i < MAX_ROW; ++i) {
        for (int j = 0; j < MAX_COL; ++j) {
            visibility[i][j] = false;
        }
    }
}
void reveal_room(Room *room, bool visibility[MAX_ROW][MAX_COL] ) {
    room->visited = true;
    for (int y = room->y; y < room->y + room->height; ++y) {
        for (int x = room->x; x < room->x + room->width; ++x) {
            visibility[y][x] = true; // Mark room area as visible
        }
    }
}
void reveal_corridor(int player_x, int player_y, int direction, bool visibility[MAX_ROW][MAX_COL] ) {
    for (int i = 0; i < 5; i++) { // Reveal 5 tiles ahead
        if (direction == 0) player_y--;  // Up
        else if (direction == 1) player_y++; // Down
        else if (direction == 2) player_x--; // Left
        else if (direction == 3) player_x++; // Right

        if (player_y < 0 || player_y >= MAX_ROW || player_x < 0 || player_x >= COLS) break;
        visibility[player_y][player_x] = true;
    }
}




