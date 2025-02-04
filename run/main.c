#include <stdatomic.h>
#include <bits/local_lim.h>

#include "essentials.h"
#include "game_menu.h"
#include "map.h"
#include "objects.h"
#include "battle.h"

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
void check_pos(int ch, char map[MAX_ROW][COLS], Room room[MAX_ROOMS], bool visibility[MAX_ROW][MAX_COL]);
void handle_movement(int ch,  char map[MAX_ROW][COLS]);
bool is_corridor(char map[MAX_ROW][COLS]);
bool is_room(char map[MAX_ROW][COLS]);
//massaging
void bottom_massage();
void clear_first_line();
void decrease_health() ;
//next floor
void on_stair(int ch, char map[MAX_ROW][COLS], Room room[MAX_ROOMS], bool visibility[MAX_ROW][MAX_COL]) ;
void next_floor(Room room[MAX_ROOMS], char map[MAX_ROW][COLS], bool visibility[MAX_ROW][MAX_COL]) ;
//visibility
void init_visibility(bool visibility[MAX_ROW][MAX_COL] );
void reveal_room(Room *room, bool visibility[MAX_ROW][MAX_COL] );
void reveal_corridor(int ch,  bool visibility[MAX_ROW][MAX_COL] ,char map[MAX_ROW][COLS]);
void check_visibility(Room room[MAX_ROOMS], bool visibility[MAX_ROW][MAX_COL],char map[MAX_ROW][COLS],  int ch) ;
int is_player_in_room(Room room[MAX_ROOMS]) ;

void print_attron(int item);
void print_map_from_array(char map[MAX_ROW][COLS], bool visibility[MAX_ROW][COLS]);

//Monster
void creat_demon(Room room[MAX_ROOMS]);

int main() {
    start_ncurses();
    start_window();
    start_music();
    init_player();
    start_menu();
    curs_set(FALSE);

    Room room[MAX_ROOMS];
    char map[MAX_ROW][COLS];

    bool visibility[MAX_ROW][MAX_COL] ;
    init_visibility(visibility);

    start_map(room, map);
    start_player_position(room);

    //place stuff
    init_demon(&demon);
    place_monster(map, room, &demon);


    reveal_room(&room[player.first_room_pos], visibility );
    // refresh();
    player.a_pressed = false;
    player.map_revealed = false;
    if (player.guest)         mvprintw(0,0,"Welcome Guest!");
    else                    mvprintw(0,0,"Welcome %s!", player.username);

    while (!player.won && !player.lost) {
        //DRAW MAP
        print_map_from_array(map, visibility);
        draw_player();
        bottom_massage();
        clear_first_line();
        decrease_health();
        // refresh();

        //MOVE PLAYER
        int ch = getch();
        if (ch == 'm' || ch == 'M') player.map_revealed = !player.map_revealed;
        check_visibility(room, visibility, map, ch);
        check_pos(ch, map, room, visibility);
        handle_movement(ch, map);
        //monster
        move_monster(map, room, &demon);
        monster_attack(&demon);

        check_game_over();
    }
    if (player.lost)    lost();
    if (player.won)      won();

}

//initializing game
void start_ncurses() {
    // setlocale(LC_ALL, "");
    setlocale(LC_CTYPE, "");
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    // nodelay(stdscr, TRUE);  // Non-blocking input
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
    int royal_blue = 10;
    init_color(royal_blue, 0, 326, 612);
    init_pair(10, royal_blue, COLOR_BLACK);
    int pink = 11;
    init_color(pink, 980, 922, 937);
    init_pair(11, pink, royal_blue);
    int silver = 12;
    init_color(silver, 768, 768, 768);
    init_pair(12, silver, COLOR_BLACK);
    int bronze = 13;
    init_color(bronze, 820,508,200);
    init_pair(13, bronze, COLOR_BLACK);
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
    // sleep(1);
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
    refresh();
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
void start_player_position(Room room[MAX_ROOMS]) {
    int which_room = rand() % 6;
    if (which_room == player.forward_stair_pos) {
        which_room = (player.forward_stair_pos + 1) % 6;
    }
    player.player_pos.x = 2 + room[which_room].x;
    player.player_pos.y = 2 + room[which_room].y;
    mvaddch(player.player_pos.y, player.player_pos.x, '@');

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

    // mvaddstr(player.player_pos.y, player.player_pos.x, L"\U0001F60D");
    // mvprintw(player.player_pos.y, player.player_pos.x, "\U0001F60D");
    // mvprintw(player.player_pos.y, player.player_pos.x, "웃");

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
void start_map(Room room[MAX_ROOMS], char map[MAX_ROW][COLS]) {
    if ((player.guest || player.creat_game_bool) && !player.resume_game_bool) {//start a new game,guest or new player
        init_map(room);

        //draw stair for first floor
        player.forward_stair_pos = rand() % 6;
        draw_forward_stair(&room[player.forward_stair_pos]);
        //place stuff
        place_gold(room);
        place_food(room);

        bottom_massage();   //for saving level and etc

        player.gold = 0;
        save_screen_to_array(map);
        save_map_to_file("map.txt", map);


    }
    else if (player.resume_game_bool && !player.creat_game_bool && !player.guest) {//continue last game, it has logged in before.
        clear();
        refresh();
        load_map_from_file("map.txt", map);
        detect_rooms(map, room);
        // Extract player stats from the last row (assuming it's at LINES-2)
        sscanf(map[MAX_ROW - 2], " Level : %d | Health : %d | Gold : %d | Food : %d/5",
                                          &player.floor, &player.health, &player.gold, &player.food_count);

    }
}
//massaging
void bottom_massage() {
    attron(A_BOLD | COLOR_PAIR(8) );  //gold
    mvprintw(MAX_ROW - 2, 0, " Level : %d | Health : %3d | Gold : %d | Food : %d/5",
                            player.floor, player.health, player.gold, player.food_count);
    attroff( A_BOLD | COLOR_PAIR(8));
    refresh();
}
void clear_first_line() {
    static time_t last_clear_time = 0;
    time_t current_time = time(NULL);

    if (current_time - last_clear_time >= 2) {  // Check if 2 seconds have passed
        // move(0, 0);     // Move cursor to the first line
        // clrtoeol();     // Clear the line from cursor to end of line
        for (int i=0; i<200; i++) {
            mvaddch(0, i, ' ');
        }

        refresh();      // Refresh screen to apply changes
        last_clear_time = current_time;  // Update last clear time
    }
}

void decrease_health() {
    int decrease_health_amount = 0;
    switch(player.game_difficulty){
        case 1:		decrease_health_amount = 2; break;
        case 2:		decrease_health_amount = 5; break;
        case 3:		decrease_health_amount = 8; break;
    }

    static time_t last_decrease_time = 0;
    time_t current_time = time(NULL);

    if (current_time - last_decrease_time >= 10) {  // Check if 10 seconds have passed
        if (player.health > 0) {
            player.health -= decrease_health_amount;  // Decrease health by 5%
            if (player.health < 0) player.health = 0;  // Ensure health doesn't go negative
        }
        last_decrease_time = current_time;  // Update last decrease time
    }
}
//next floor
void on_stair(int ch, char map[MAX_ROW][COLS], Room room[MAX_ROOMS], bool visibility[MAX_ROW][MAX_COL]) {
        if (ch == 'a' || ch == 'A') { // Detect 'A' key
            player.a_pressed = true;         // Set flag when 'A' is detected
        } else if (player.a_pressed && ch == KEY_LEFT) {
            player.a_pressed = false; // Reset flag
            //make next floor
            next_floor(room , map, visibility);
        }
        else {
            player.a_pressed = false; // Reset flag if another key is pressed
        }

}
void next_floor(Room room[MAX_ROOMS], char map[MAX_ROW][COLS], bool visibility[MAX_ROW][MAX_COL]) {
    //make new map
    init_map(room);

    //initialize visibility
    init_visibility(visibility);
    //update player floor
    player.floor ++;
    bottom_massage();   //for saving level

    player.backward_stair_pos = player.forward_stair_pos;
    //draw stair to last floor & player new position
    draw_backward_stair_player_pos(&room[player.backward_stair_pos]);
    //show current room
    reveal_room(&room[player.backward_stair_pos], visibility);
    place_gold(room);
    place_food(room);
    if (player.floor == 3 || player.floor == 4)  place_dark_gold(room);



    //draw random stair for next floor in different room
    if (player.floor < 4) {
        int stair_new_pos = rand() % 6;
        // Ensure the new room for stair is not placed in the same room as last stair
        if (stair_new_pos == player.forward_stair_pos) {
            player.forward_stair_pos = (stair_new_pos + 1) % 6;
        }else {
            player.forward_stair_pos = stair_new_pos;
        }

        draw_forward_stair(&room[player.forward_stair_pos]);
    }


    save_screen_to_array(map);
    save_map_to_file("map.txt", map);
    mvprintw(0,0, "Welcome to new floor!");


    init_demon(&demon);
    place_monster(map, room, &demon);

}
//movement
void check_pos(int ch, char map[MAX_ROW][COLS], Room room[MAX_ROOMS], bool visibility[MAX_ROW][MAX_COL]) {
    int current_pos = map[player.player_pos.y][player.player_pos.x];

    if (current_pos == '<' )    on_stair(ch, map, room, visibility);    //standing on a forward stair
    if (current_pos == '$')     get_gold(map);
    if (current_pos == '"')     get_dark_gold(map);
    if (current_pos == '&')     get_food(map);

    if (current_pos == '<')      mvprintw(0,0,"It's a stair to next floor! Press A + Key_Left to go to next floor.");

}
void handle_movement(int ch, char map[MAX_ROW][COLS]){

    int next_move = ' ';
    int do_nothing = 0;
    if (ch == 'q' || ch == 'Q') {
        endwin();
        exit(0);
    }
    if (ch == 'k' || ch == 'K' || ch == '8' || ch == KEY_UP) {      //move up
        next_move = map[player.player_pos.y - 1][player.player_pos.x];
        if (next_move == ' ' || next_move == '|' || next_move == '_' || next_move == '-' || next_move == 'D')
            return;   //do nothing
        // if (  (is_corridor(map) && (next_move == '#' || next_move == '+')) || is_room(map) )
            player.player_pos.y -- ;
        }
    else if (ch == 'j' || ch == 'J' || ch == '2' || ch == KEY_DOWN) {   //move down
        next_move = map[player.player_pos.y + 1][player.player_pos.x];
        if ( next_move == ' ' || next_move == '|' || next_move == '_' || next_move == '-'|| next_move == 'D')
                return;   //do nothing
        // if (  (is_corridor(map) && (next_move == '#' || next_move == '+')) || is_room(map) )
                player.player_pos.y ++ ;
        }
    else if (ch == 'h' || ch == 'H' || ch == '4' || ch == KEY_LEFT) {     //move left
        next_move = map[player.player_pos.y][player.player_pos.x - 1];

        if (next_move == ' ' || next_move == '|' || next_move == '_' || next_move == '-'|| next_move == 'D')
                return;   //do nothing
        // if (  (is_corridor(map) && (next_move == '#' || next_move == '+')) || is_room(map) )
                player.player_pos.x -- ;
        }
    else if (ch == 'l' || ch == 'L' || ch == '6' || ch == KEY_RIGHT) {   //move right
        next_move = map[player.player_pos.y][player.player_pos.x + 1];

        if (next_move == ' ' || next_move == '|' || next_move == '_' || next_move == '-'|| next_move == 'D')
                return;   //do nothing
        // if (  (is_corridor(map) && (next_move == '#' || next_move == '+')) || is_room(map) )
                    player.player_pos.x ++ ;

        }
    else if (ch == 'u' || ch == 'U' ) {   //اریب بالا راست
        next_move = map[player.player_pos.y -1][player.player_pos.x +1];

        if (next_move == ' ' || next_move == '|' || next_move == '_' || next_move == '-') {
                return;   //do nothing
            }
        if (  (is_corridor(map) && (next_move == '#' || next_move == '+')) || is_room(map) ){
                player.player_pos.y -- ;
                player.player_pos.x ++ ;
            }
        }
    else if (ch == 'y' || ch == 'Y' ){   //اریب بالا چپ
        next_move = map[player.player_pos.y -1][player.player_pos.x -1];

        if (next_move == ' ' || next_move == '|' || next_move == '_' || next_move == '-') {
                return;   //do nothing
            }
        if (  (is_corridor(map) && (next_move == '#' || next_move == '+')) || is_room(map) ){
                player.player_pos.y -- ;
                player.player_pos.x -- ;
            }
        }
    else if (ch == 'n' || ch == 'N' ) {   //اریب پایین راست
        next_move = map[player.player_pos.y +1][player.player_pos.x +1];

        if (next_move == ' ' || next_move == '|' || next_move == '_' || next_move == '-') {
                return;   //do nothing
            }
        if (  (is_corridor(map) && (next_move == '#' || next_move == '+')) || is_room(map) ) {
                player.player_pos.y ++ ;
                player.player_pos.x ++ ;
            }
        }
    else if (ch == 'b' || ch == 'B' ) {   //اریب پایین چپ
        next_move = map[player.player_pos.y +1][player.player_pos.x -1];

        if (next_move == ' ' || next_move == '|' || next_move == '_' || next_move == '-') {
                return;   //do nothing
            }
        if (  (is_corridor(map) && (next_move == '#' || next_move == '+')) || is_room(map) ){
                player.player_pos.y ++ ;
                player.player_pos.x -- ;
            }
        }
    else if (ch == 'a' || ch == 'A'
          || ch == 'm' || ch == 'M') {
        return;
    }

    else {
            //mvprintw(0, 0, "Invalid Command");
            //refresh();
            //sleep(0.5);
        }

    if (next_move == '<')      mvprintw(0,0,"It's a stair to next floor! Press A + Key_Left to go to next floor.");
    if (next_move == '>')      mvprintw(0,0,"Stairs to last floor");
    if (next_move == '$')      mvprintw(0,0,"You found 5 gold coins! Your fortune grows!");
    if (next_move == '"')      mvprintw(0,0,"You found a DARK gold! 10 coins added to your gold.");
    if (next_move == '&')      mvprintw(0,0,"Yummy! Health increased.");



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
void reveal_corridor(int ch,  bool visibility[MAX_ROW][MAX_COL] ,char map[MAX_ROW][COLS]) {
    int dx = 0, dy = 0;

    // Determine direction of movement
    switch (ch) {
        case KEY_UP:    dy = -1; break;
        case KEY_DOWN:  dy = 1; break;
        case KEY_LEFT:  dx = -1; break;
        case KEY_RIGHT: dx = 1; break;
    }

    // Reveal 5 tiles in the direction the player is facing
    for (int i = 1; i <= 5; i++) {
        int nx = player.player_pos.x + (dx * i);
        int ny = player.player_pos.y + (dy * i);

        if (nx < 0 || nx >= COLS || ny < 0 || ny >= MAX_ROW) break; // Prevent out-of-bounds
        if (map[ny][nx] == '#') { // Only reveal corridors
        // if (mvinch(ny, nx) == '#'){
            visibility[ny][nx] = true;
        } else {
            break; // Stop revealing if it's not a corridor
        }
    }
}
int is_player_in_room(Room room[MAX_ROOMS]) {
    for (int i = 0; i < MAX_ROOMS; i++) {  // Loop through all rooms
        // Check if player's coordinates are inside the room boundaries
        if (player.player_pos.x >= room[i].x && player.player_pos.x < room[i].x + room[i].width &&
            player.player_pos.y >= room[i].y && player.player_pos.y < room[i].y + room[i].height) {
            return i;  // Player is inside this room
            }
    }
    return -1;  // Player is not inside any room
}
void check_visibility(Room room[MAX_ROOMS], bool visibility[MAX_ROW][MAX_COL],char map[MAX_ROW][COLS],  int ch) {
    int room_number = is_player_in_room(room);
    if (room_number != -1) {
        reveal_room(&room[room_number], visibility);
    }
    // if (mvinch(player.player_pos.y , player.player_pos.x) == '+' || mvinch(player.player_pos.y , player.player_pos.x) == '#') {
    if (map[player.player_pos.y][player.player_pos.x] == '+' || map[player.player_pos.y][player.player_pos.x] == '#'){
        reveal_corridor(ch, visibility, map);
    }
}
//print map
void print_attron(int item) {
    switch (item) {
        case '#':   attron(COLOR_PAIR(9)); break;
        case '$':   attron(A_BOLD | COLOR_PAIR(8)); break; //gold
        case '"':   attron(A_BOLD | COLOR_PAIR(10)); break; //dark gold
        case '&':   attron(A_BOLD | COLOR_PAIR(4)); break; //food
        case 'D':   attron(A_BOLD | COLOR_PAIR(5)); break; //demon monster
    }
}
void print_map_from_array(char map[MAX_ROW][COLS], bool visibility[MAX_ROW][COLS]) {
    for (int y = 1; y < MAX_ROW; ++y) {
        for (int x = 0; x < COLS; ++x) {
            if (player.map_revealed) {
                print_attron(map[y][x]);
                mvaddch(y, x, map[y][x]);
                attroff(A_BOLD | COLOR_PAIR(4) | COLOR_PAIR(7) | COLOR_PAIR(8) | COLOR_PAIR(9) | COLOR_PAIR(10));
            }
            else {
                if (visibility[y][x]) { // Show revealed areas
                    print_attron(map[y][x]);
                    mvaddch(y, x, map[y][x]);
                    attroff(A_BOLD | COLOR_PAIR(4) | COLOR_PAIR(7) | COLOR_PAIR(8) | COLOR_PAIR(9) | COLOR_PAIR(10));
                } else {
                    mvaddch(y, x, ' '); // Hide unseen areas
                }
            }
        }
    }
    // draw bottom line of the page
    char *bottom_line = "_ . ";
    int pattern_length = 4;
    for (int i = 0; i < COLS; i += pattern_length) {
        mvprintw(LINES - 4, i, bottom_line);
    }
}

//Monster
// void creat_demon(Room room[MAX_ROOMS]) {
//     Monster demon;
//     init_demon(&demon);
//     place_monster(room, &demon);
// }