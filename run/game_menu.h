#ifndef GAME_MENU_H
#define GAME_MENU_H

#include "essentials.h"



Player player;
int difficulty_level = 2; //default: medium

//prototypes
void start_menu();
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
void game_setting(MENU *menu, WINDOW *menu_win);
void game_difficulty(MENU *menu, WINDOW *menu_win);
void player_color(MENU *menu, WINDOW *menu_win);
void before_you_play(MENU *menu, WINDOW *menu_win);
void resume_game(MENU *menu, WINDOW *menu_win);
void new_game(MENU *menu, WINDOW *menu_win);
void select_music(MENU *menu, WINDOW *menu_win);
void play_music(const char *filename) ;
void scoreboard(MENU *menu, WINDOW *menu_win) ;
void show_leaderboard(const char *filename) ;


void start_menu() {
    // clear();
    // refresh();
    player.logged_in = false;
    player.guest = false;
    player.creat_game_bool = false;
    player.resume_game_bool = false;
    player.floor = 1;
    player.game_difficulty = 2;// default: medium


    char *choices[] = {
        "1. Rogue Login",
        "2. Create New Rogue",
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

    refresh();
    // Post the menu
    post_menu(menu);
    wrefresh(menu_win);

    // Menu interaction loop
    while ((c = wgetch(menu_win)) != 'q' ) {
        if (player.resume_game_bool || player.creat_game_bool) break;
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
                    endwin();
                    exit(0);

                }
                else if (strcmp(item_name(cur), "2. Create New Rogue") == 0) {
                    create_new_rogue(menu, menu_win);
                }
                else if (strcmp(item_name(cur), "1. Rogue Login") == 0) {
                    login_rogue(menu, menu_win);
                }
                else if (strcmp(item_name(cur), "3. Before You Play") == 0) {
                    before_you_play(menu, menu_win);
                }

            }
                break;

        }

    }
    if (player.resume_game_bool || player.creat_game_bool){
        unpost_menu(menu);
        free_menu(menu);
        for (i = 0; i < n_choices; ++i)
            free_item(items[i]);

    }
}

//function to print text in the middle of a window
void print_in_middle(WINDOW *win, int starty, int startx, int width, char *string, chtype color) {
    int length, x, y;
    float temp;

    if (win == NULL)
        win = stdscr;
    getyx(win, y, x);
    if (startx != 0)
        x = startx;
    if (starty != 0)
        y = starty;
    if (width == 0)
        width = 80;

    length = strlen(string);
    temp = (width - length) / 2;
    x = startx + (int)temp;
    wattron(win, color);
    mvwprintw(win, y, x, "%s", string);
    wattroff(win, color);
    wrefresh(win);
}

// "Create New Rogue" option
void create_new_rogue(MENU *menu, WINDOW *menu_win) {
    clear();
    WINDOW *form_win = newwin(14, 50, (LINES - 12) / 2, (COLS - 50) / 2);
    box(form_win, 0, 0);
    wattron(form_win, COLOR_PAIR(1));
    mvwprintw(form_win, 1, 16, "Create New Rogue");
    wattroff(form_win, COLOR_PAIR(1));
    mvwhline(form_win, 2, 1, ACS_HLINE, 48);
    mvwprintw(form_win, 3, 2, "Enter Username:");
    mvwprintw(form_win, 5, 2, "Enter Password:");
    mvwprintw(form_win, 7, 2, "Enter Email:");

    char username[30], password[30], email[60];
    echo();

    const char *filename = "users.txt";
    while (1){
        mvwgetnstr(form_win, 3, 18, username, 29);

        if (username_exists(filename, username)) {
            wattron(form_win, COLOR_PAIR(1));
            mvwprintw(form_win, 9, 3, "Username already exists! Try again.");
            wattroff(form_win, COLOR_PAIR(1));
            wrefresh(form_win);
            wclrtoeol(form_win);
            mvwprintw(form_win, 3, 18, "                            ");
            wrefresh(form_win);
        } else{
            break;
        }

    }   //validate username
    mvwprintw(form_win, 9, 3, "                                             ");

    while (1) {
        wattron(form_win, COLOR_PAIR(2));
        mvwprintw(form_win, 10, 3, "'R' for a random pass or enter your own");
        wattroff(form_win, COLOR_PAIR(2));
        mvwprintw(form_win, 5, 18, "                         ");  // Clear input field
        wrefresh(form_win);

        noecho();
        int ch = wgetch(form_win);
        echo();
        if (ch == 'R' || ch == 'r') {
            strcpy(password, generate_random_password());
            mvwprintw(form_win, 5, 18, "%s", password);  // display generated password
            break;
        } else {
            mvwprintw(form_win, 9, 3, "                                             ");
            mvwgetnstr(form_win, 5, 18, password, 29);

            if ((strlen(password) > 7) && contains_uppercase(password) && contains_lowercase(password) && contains_number(password)) {
                break;
            } else {
                wattron(form_win, COLOR_PAIR(1));
                mvwprintw(form_win, 9, 3, "Invalid Password! ");
                wattroff(form_win, COLOR_PAIR(1));
                wrefresh(form_win);
            }
        }
    }   //validate password
    mvwprintw(form_win, 9, 3, "                                             ");
    mvwprintw(form_win, 10, 3, "                                             ");

    while (1) {     //check if email is valid
        mvwgetnstr(form_win, 7, 15, email, 29);

        if (is_valid_email(email)) {
            break;
        } else {
            wattron(form_win, COLOR_PAIR(1));
            mvwprintw(form_win, 9, 3, "Invalid Email Format!");
            wattroff(form_win, COLOR_PAIR(1));
            wrefresh(form_win);
            wclrtoeol(form_win);
            mvwprintw(form_win, 7, 15, "             ");  // Clear the email input line
            wrefresh(form_win);
        }
    }   //validate email

    noecho();


    if (username_exists(filename, username)) {
        wattron(form_win, COLOR_PAIR(1));
        mvwprintw(form_win, 9, 3, "Username Already Exists! Try Again.");
        wattroff(form_win, COLOR_PAIR(1));
    }
    else {
        register_user(filename, username, password);
        wattron(form_win, COLOR_PAIR(2));
        mvwprintw(form_win, 10, 2, "User Registered Successfully!");
        wattroff(form_win, COLOR_PAIR(2));
        wrefresh(form_win);
        // wgetch(form_win);

        strcpy(player.username, username);
        strcpy(player.password, password);
        strcpy(player.email, email);
        player.logged_in = true;


    }
    wattron(form_win, COLOR_PAIR(2));
    mvwprintw(form_win, 11, 3, "Go To Pre-Game Menu...");
    wattroff(form_win, COLOR_PAIR(2));
    wrefresh(form_win);
    wgetch(form_win);

    delwin(form_win);
    clear();
    refresh();
    // Clear and redisplay the menu
    set_menu_win(menu, menu_win);
    set_menu_mark(menu, " * ");
    // Print a border and title
    box(menu_win, 0, 0);
    print_in_middle(menu_win, 1, 0, 40, "Game Menu", COLOR_PAIR(1));
    mvwaddch(menu_win, 2, 0, ACS_LTEE);
    mvwhline(menu_win, 2, 1, ACS_HLINE, 40 - 2);
    mvwaddch(menu_win, 2, 40 - 1, ACS_RTEE);
    refresh();
    // Post the menu
    post_menu(menu);
    wrefresh(menu_win);
    return;
}

bool contains_number(const char *password) {
    for (int i = 0; password[i] != '\0'; i++) {
        if (isdigit(password[i])) {
            return true;  // Found a digit
        }
    }
    return false;  // No digits found
}
bool contains_uppercase(const char *password) {
    for (int i = 0; password[i] != '\0'; i++) {
        if (isupper(password[i])) {
            return true;  // Found a capital letter
        }
    }
    return false;  // No capital letter found
}
bool contains_lowercase(const char *password) {
    for (int i = 0; password[i] != '\0'; i++) {
        if (islower(password[i])) {
            return true;  // Found a lowercase letter
        }
    }
    return false;  // No lowercase letter found
}
char *generate_random_password() {
    static char password[30];
    const char *lowercase = "abcdefghijklmnopqrstuvwxyz";
    const char *uppercase = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const char *digits = "0123456789";
    const char *special = "!@#$%^&*";

    // Seed the random number generator
    srand(time(NULL));

    // Ensure at least one character of each type
    password[0] = lowercase[rand() % strlen(lowercase)];
    password[1] = uppercase[rand() % strlen(uppercase)];
    password[2] = digits[rand() % strlen(digits)];
    password[3] = special[rand() % strlen(special)];

    // Fill the rest with random characters
    const char *all_chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*";
    for (int i = 4; i < 12; i++) {  // Generate a 12-character password
        password[i] = all_chars[rand() % strlen(all_chars)];
    }

    password[12] = '\0';  // Null-terminate the password

    // Shuffle the password for randomness
    for (int i = 0; i < 12; i++) {
        int j = rand() % 12;
        char temp = password[i];
        password[i] = password[j];
        password[j] = temp;
    }

    return password;
}
bool is_valid_email(const char *email) {
    const char *at = strchr(email, '@');
    const char *dot = strrchr(email, '.');

    // Check if '@' and '.' are present and in the correct order
    if (at && dot && at < dot && dot - at > 1 && strlen(dot) > 1) {
        return true;
    }
    return false;
}
bool username_exists(const char *filename, const char *username) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Could not open file");
        return false;
    }

    char line[MAX_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        // Extract username from the line
        char stored_username[MAX_LENGTH];
        sscanf(line, "%[^:]", stored_username);  // Read up to ':'
        if (strcmp(stored_username, username) == 0) {
            fclose(file);
            return true;  // Username already exists
        }
    }
    fclose(file);
    return false;  // Username not found
}
void register_user(const char *filename, const char *username, const char *password) {
    if (username_exists(filename, username)) {
        printf("Username '%s' already exists. Please choose another.\n", username);
        return;
    }

    FILE *file = fopen(filename, "a");
    if (!file) {
        perror("Could not open file");
        return;
    }

    fprintf(file, "%s:%s:0\n", username, password);  // Store username and password
    fclose(file);
//    printf("User '%s' registered successfully!\n", username);
}


// "Rogue Login" menu
void login_rogue(MENU *menu, WINDOW *menu_win) {
    clear();
    WINDOW *form_win = newwin(14, 50, (LINES - 12) / 2, (COLS - 50) / 2);
    box(form_win, 0, 0);
    wattron(form_win, COLOR_PAIR(1));
    mvwprintw(form_win, 1, 16, "Rogue Login");
    wattroff(form_win, COLOR_PAIR(1));

    mvwhline(form_win, 2, 1, ACS_HLINE, 48);
    mvwprintw(form_win, 3, 2, "Enter Username:");
    mvwprintw(form_win, 5, 2, "Enter Password:");

    wattron(form_win, COLOR_PAIR(2));
    mvwprintw(form_win, 9, 2, "Enter Username or press 'G' for Guest Player");
    wattroff(form_win, COLOR_PAIR(2));
    char username[30], password[30];
    echo();

    mvwgetnstr(form_win, 3, 18, username, 29);

    // Check if user wants to log in as Guest
    if (strlen(username) == 1 && (username[0] == 'G' || username[0] == 'g')) {
        player.guest = true;
        player.logged_in = true;

        mvwprintw(form_win, 9, 2, "                                             ");
        wattron(form_win, COLOR_PAIR(2));
        mvwprintw(form_win, 9, 3, "Logged in as Guest Player!");
        mvwprintw(form_win, 11, 3, "Go To Pre-Game Menu...");
        wattroff(form_win, COLOR_PAIR(2));
        wrefresh(form_win);
        wgetch(form_win);

        delwin(form_win);
        clear();
        refresh();
        // Clear and redisplay the menu
        set_menu_win(menu, menu_win);
        set_menu_mark(menu, " * ");
        // Print a border and title
        box(menu_win, 0, 0);
        print_in_middle(menu_win, 1, 0, 40, "Game Menu", COLOR_PAIR(1));
        mvwaddch(menu_win, 2, 0, ACS_LTEE);
        mvwhline(menu_win, 2, 1, ACS_HLINE, 40 - 2);
        mvwaddch(menu_win, 2, 40 - 1, ACS_RTEE);
        refresh();
        // Post the menu
        post_menu(menu);
        wrefresh(menu_win);
        return;
    }

    mvwgetnstr(form_win, 5, 18, password, 29);

    noecho();

    // Verify username and password
    const char *filename = "users.txt";
    if (validate_credentials(filename, username, password)) {
        strcpy(player.username, username);
        strcpy(player.password, password);
        player.logged_in = true;

        mvwprintw(form_win, 9, 2, "                                            ");
        wattron(form_win, COLOR_PAIR(2));
        mvwprintw(form_win, 9, 3, "Welcome %s!", username);
        mvwprintw(form_win, 11, 3, "Go To Pre-Game Menu...");
        wattroff(form_win, COLOR_PAIR(2));
        wrefresh(form_win);
        wgetch(form_win);

        delwin(form_win);
        clear();
        refresh();
        // Clear and redisplay the menu
        set_menu_win(menu, menu_win);
        set_menu_mark(menu, " * ");
        // Print a border and title
        box(menu_win, 0, 0);
        print_in_middle(menu_win, 1, 0, 40, "Game Menu", COLOR_PAIR(1));
        mvwaddch(menu_win, 2, 0, ACS_LTEE);
        mvwhline(menu_win, 2, 1, ACS_HLINE, 40 - 2);
        mvwaddch(menu_win, 2, 40 - 1, ACS_RTEE);

        refresh();
        // Post the menu
        post_menu(menu);
        wrefresh(menu_win);
        return;

    }else {
        mvwprintw(form_win, 9, 2, "                                             ");
        wattron(form_win, COLOR_PAIR(1));
        mvwprintw(form_win, 9, 3, "Invalid username or password!");
        wattroff(form_win, COLOR_PAIR(1));

        wattron(form_win, COLOR_PAIR(2));
        mvwprintw(form_win, 11, 3, "Press any key to return to the menu...");
        wattroff(form_win, COLOR_PAIR(2));
        wrefresh(form_win);
        wgetch(form_win);

        delwin(form_win);
        clear();
        refresh();
        // Clear and redisplay the menu
        set_menu_win(menu, menu_win);
        set_menu_mark(menu, " * ");
        // Print a border and title
        box(menu_win, 0, 0);
        print_in_middle(menu_win, 1, 0, 40, "Game Menu", COLOR_PAIR(1));
        mvwaddch(menu_win, 2, 0, ACS_LTEE);
        mvwhline(menu_win, 2, 1, ACS_HLINE, 40 - 2);
        mvwaddch(menu_win, 2, 40 - 1, ACS_RTEE);
        refresh();
        // Post the menu
        post_menu(menu);
        wrefresh(menu_win);
        return;

    }


}
bool validate_credentials(const char *filename, const char *username, const char *password) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Could not open file");
        return false;
    }

    char line[100];
    while (fgets(line, sizeof(line), file)) {
        char stored_username[30], stored_password[30];
        sscanf(line, "%[^:]:%s", stored_username, stored_password);
        if (strcmp(stored_username, username) == 0 && strcmp(stored_password, password) == 0) {
            fclose(file);
            return true;
        }
    }

    fclose(file);
    return false;
}

//"Before You Play" menu
void before_you_play(MENU *menu, WINDOW *menu_win) {
    clear();
    refresh();
    char *sub_choices[] = {
            "1. New Game",
            "2. Resume Game",
            "3. Score Board",
            "4. Setting",
            "5. Back",
            NULL
    };

    int n_sub_choices = ARRAY_SIZE(sub_choices) - 1;
    ITEM **sub_items = (ITEM **)calloc(n_sub_choices + 1, sizeof(ITEM *));
    for (int i = 0; i < n_sub_choices; ++i)
        sub_items[i] = new_item(sub_choices[i], NULL);
    sub_items[n_sub_choices] = NULL;

    MENU *sub_menu = new_menu((ITEM **)sub_items);

    // Create submenu window
    WINDOW *sub_menu_win = newwin(12, 40, (LINES - 12) / 2, (COLS - 40) / 2);
    keypad(sub_menu_win, TRUE);

    set_menu_win(sub_menu, sub_menu_win);
    set_menu_sub(sub_menu, derwin(sub_menu_win, 8, 38, 3, 1));
    set_menu_mark(sub_menu, " > ");

    // Print a border and title
    box(sub_menu_win, 0, 0);
    print_in_middle(sub_menu_win, 1, 0, 40, "Before You Play", COLOR_PAIR(1));
    mvwaddch(sub_menu_win, 2, 0, ACS_LTEE);
    mvwhline(sub_menu_win, 2, 1, ACS_HLINE, 38);
    mvwaddch(sub_menu_win, 2, 39, ACS_RTEE);

    // Post the menu
    post_menu(sub_menu);
    wrefresh(sub_menu_win);
    int c;
    while ((c = wgetch(sub_menu_win)) != 'q') {
        if (player.resume_game_bool || player.creat_game_bool)  break;
        switch (c) {
            case KEY_DOWN:
                menu_driver(sub_menu, REQ_DOWN_ITEM);
                break;
            case KEY_UP:
                menu_driver(sub_menu, REQ_UP_ITEM);
                break;
            case 10: {  // Enter key
                ITEM *cur = current_item(sub_menu);
                const char *name = item_name(cur);
                if (strcmp(name, "5. Back") == 0) {
                    // Cleanup
                    unpost_menu(sub_menu);
                    free_menu(sub_menu);
                    for (int i = 0; i < n_sub_choices; ++i)
                        free_item(sub_items[i]);
                    delwin(sub_menu_win);

                    // Return to main menu
                    clear();
                    refresh();
                    // Print a border and title
                    box(menu_win, 0, 0);
                    print_in_middle(menu_win, 1, 0, 40, "Game Menu", COLOR_PAIR(1));
                    mvwaddch(menu_win, 2, 0, ACS_LTEE);
                    mvwhline(menu_win, 2, 1, ACS_HLINE, 40 - 2);
                    mvwaddch(menu_win, 2, 40 - 1, ACS_RTEE);

                    refresh();
                    // Post the menu
                    post_menu(menu);
                    wrefresh(menu_win);
                    post_menu(menu);
                    wrefresh(menu_win);
                    return;
                    // start_menu();
                }
                else if (strcmp(name,"1. New Game") == 0) {
                    new_game(sub_menu, sub_menu_win);

                } else if (strcmp(name, "2. Resume Game") == 0) {
                    resume_game(sub_menu, sub_menu_win);

                } else if (strcmp(name, "3. Score Board") == 0) {
                    show_leaderboard("users.txt");
                    // Re-draw the menu after returning from the leaderboard
                    clear();
                    refresh();
                    box(sub_menu_win, 0, 0);
                    print_in_middle(sub_menu_win, 1, 0, 40, "Before You Play", COLOR_PAIR(1));
                    mvwaddch(sub_menu_win, 2, 0, ACS_LTEE);
                    mvwhline(sub_menu_win, 2, 1, ACS_HLINE, 38);
                    mvwaddch(sub_menu_win, 2, 39, ACS_RTEE);
                    post_menu(sub_menu);
                    wrefresh(sub_menu_win);


                } else if (strcmp(name, "4. Setting") == 0) {
                    game_setting(sub_menu, sub_menu_win);
                }
                wclrtoeol(sub_menu_win);
                wrefresh(sub_menu_win);
            }
                break;
        }
    }
}
void new_game(MENU *menu, WINDOW *menu_win) {
    if (player.logged_in == true) {
        unpost_menu(menu);
        free_menu(menu);
        delwin(menu_win);
        clear();
        refresh();
        mvwprintw(menu_win, 9, 3, "DONE");
        player.creat_game_bool = true;
        return;
    }

    // if (player.logged_in == false)
    wattron(menu_win, COLOR_PAIR(2));
    mvwprintw(menu_win, 9, 3, "Please Login First");
    wattroff(menu_win, COLOR_PAIR(2));
    wrefresh(menu_win);
    wgetch(menu_win);
}
void resume_game(MENU *menu, WINDOW *menu_win) {
    if (player.guest) {
        wattron(menu_win, COLOR_PAIR(2));
        mvwprintw(menu_win, 9, 3, "You Don't Have Any Game History");
        mvwprintw(menu_win, 10, 3, "Start a New Game");
        wattroff(menu_win, COLOR_PAIR(2));
        wrefresh(menu_win);
        wgetch(menu_win);
        return;
    }
    if (player.logged_in) {
        unpost_menu(menu);
        free_menu(menu);
        delwin(menu_win);
        clear();
        refresh();
        mvwprintw(menu_win, 9, 3, "Let's Go");
        player.resume_game_bool = true;
        return;
    }

    // if (player.logged_in == false)
    wattron(menu_win, COLOR_PAIR(2));
    mvwprintw(menu_win, 9, 3, "Please Login First");
    wattroff(menu_win, COLOR_PAIR(2));
    wrefresh(menu_win);
    wgetch(menu_win);
}

//"Setting" menu
void game_setting(MENU *menu, WINDOW *menu_win) {
    clear();
    refresh();
    char *setting_options[] = {
        "1. Game Difficulty",
        "2. Rogue Color",
        "3. Choose Music",
        "4. Back",
        NULL
        };
    int n_setting_choices = ARRAY_SIZE(setting_options) - 1;
    ITEM **setting_items = (ITEM **)calloc(n_setting_choices + 1, sizeof(ITEM *));
    for (int i = 0; i < n_setting_choices; ++i)
        setting_items[i] = new_item(setting_options[i], NULL);
    setting_items[n_setting_choices] = NULL;

    MENU *setting_menu = new_menu((ITEM **)setting_items);

    // Create setting window
    WINDOW *setting_menu_win = newwin(12, 40, (LINES - 12) / 2, (COLS - 40) / 2);
    keypad(setting_menu_win, TRUE);

    set_menu_win(setting_menu, setting_menu_win);
    set_menu_sub(setting_menu, derwin(setting_menu_win, 8, 38, 3, 1));
    set_menu_mark(setting_menu, " > ");

    // Print a border and title
    box(setting_menu_win, 0, 0);
    print_in_middle(setting_menu_win, 1, 0, 40, "Setting", COLOR_PAIR(1));
    mvwaddch(setting_menu_win, 2, 0, ACS_LTEE);
    mvwhline(setting_menu_win, 2, 1, ACS_HLINE, 38);
    mvwaddch(setting_menu_win, 2, 39, ACS_RTEE);

    // Post the menu
    post_menu(setting_menu);
    wrefresh(setting_menu_win);

    int c;
    while ((c = wgetch(setting_menu_win)) != 'q') {
        switch (c) {
            case KEY_DOWN:
                menu_driver(setting_menu, REQ_DOWN_ITEM);
            break;
            case KEY_UP:
                menu_driver(setting_menu, REQ_UP_ITEM);
            break;
            case 10: {  // Enter key
                ITEM *cur = current_item(setting_menu);
                const char *name = item_name(cur);

                if (strcmp(name, "4. Back") == 0) {
                    // Cleanup
                    unpost_menu(setting_menu);
                    free_menu(setting_menu);
                    for (int i = 0; i < n_setting_choices; ++i)
                        free_item(setting_items[i]);
                    delwin(setting_menu_win);

                    // Return to main menu
                    clear();
                    refresh();
                    // set_menu_win(menu, menu_win);
                    // set_menu_sub(menu, derwin(menu_win, 8, 38, 3, 1));
                    // set_menu_mark(menu, " > ");

                    // Print a border and title
                    box(menu_win, 0, 0);
                    print_in_middle(menu_win, 1, 0, 40, "Before You Play", COLOR_PAIR(1));
                    mvwaddch(menu_win, 2, 0, ACS_LTEE);
                    mvwhline(menu_win, 2, 1, ACS_HLINE, 38);
                    mvwaddch(menu_win, 2, 39, ACS_RTEE);

                    // Post the menu
                    post_menu(menu);
                    wrefresh(menu_win);
                    return;

                    // post_menu(menu);
                    // wrefresh(menu_win);
                    // return;
                    // before_you_play(menu, menu_win);

                }
                else if (strcmp(name,"1. Game Difficulty") == 0) {
                    game_difficulty(setting_menu, setting_menu_win);

                }
                else if (strcmp(name, "2. Rogue Color") == 0) {
                    player_color(setting_menu, setting_menu_win);

                }
                else if (strcmp(name, "3. Choose Music") == 0) {
                    select_music(setting_menu, setting_menu_win);
                }
                wclrtoeol(setting_menu_win);
                wrefresh(setting_menu_win);
            }
            break;
        }
    }



}
void game_difficulty(MENU *menu, WINDOW *menu_win) {
    clear();
    refresh();
    char *difficulty_choices[] = {
        "1. Easy",
        "2. Medium",
        "3. Hard",
        "4. Back",
        NULL,
    };

    int n_sub_choices = ARRAY_SIZE(difficulty_choices) - 1;
    ITEM **sub_items = (ITEM **)calloc(n_sub_choices + 1, sizeof(ITEM *));
    for (int i = 0; i < n_sub_choices; ++i)
        sub_items[i] = new_item(difficulty_choices[i], NULL);
    sub_items[n_sub_choices] = NULL;

    MENU *sub_menu = new_menu((ITEM **)sub_items);
    // Create submenu window
    WINDOW *sub_menu_win = newwin(12, 40, (LINES - 12) / 2, (COLS - 40) / 2);
    keypad(sub_menu_win, TRUE);

    set_menu_win(sub_menu, sub_menu_win);
    set_menu_sub(sub_menu, derwin(sub_menu_win, 8, 38, 3, 1));
    set_menu_mark(sub_menu, " > ");

    // Print a border and title
    box(sub_menu_win, 0, 0);
    print_in_middle(sub_menu_win, 1, 0, 40, "Game Difficulty", COLOR_PAIR(1));
    mvwaddch(sub_menu_win, 2, 0, ACS_LTEE);
    mvwhline(sub_menu_win, 2, 1, ACS_HLINE, 38);
    mvwaddch(sub_menu_win, 2, 39, ACS_RTEE);
    refresh();

    // Post the menu
    post_menu(sub_menu);
    wrefresh(sub_menu_win);
    refresh();




    char selected_difficulty[20] = "Medium";  // Default difficulty
    int c;
    while ((c = wgetch(sub_menu_win)) != 'q') {
        switch (c) {
            case KEY_DOWN:
                menu_driver(sub_menu, REQ_DOWN_ITEM);
                break;
            case KEY_UP:
                menu_driver(sub_menu, REQ_UP_ITEM);
                break;
            case 10: {  // Enter key
                ITEM *cur = current_item(sub_menu);
                const char *name = item_name(cur);
                wattron(sub_menu_win, COLOR_PAIR(2));


                if (strcmp(name, "4. Back") == 0) {
                    // Cleanup
                    unpost_menu(sub_menu);
                    free_menu(sub_menu);
                    for (int i = 0; i < n_sub_choices; ++i)
                        free_item(sub_items[i]);
                    delwin(sub_menu_win);

                    // Return to main menu
                    clear();
                    refresh();
                    set_menu_win(menu, menu_win);
                    set_menu_sub(menu, derwin(menu_win, 8, 38, 3, 1));
                    set_menu_mark(menu, " > ");

                    // Print a border and title
                    box(menu_win, 0, 0);
                    print_in_middle(menu_win, 1, 0, 40, "Setting", COLOR_PAIR(1));
                    mvwaddch(menu_win, 2, 0, ACS_LTEE);
                    mvwhline(menu_win, 2, 1, ACS_HLINE, 38);
                    mvwaddch(menu_win, 2, 39, ACS_RTEE);

                    // Post the menu
                    post_menu(menu);
                    wrefresh(menu_win);
                    return;

                } else if (strcmp(name,"1. Easy") == 0) {
                    mvwprintw(sub_menu_win, 9, 4, "Difficulty: EASY");
                    player.game_difficulty = 1;

                } else if (strcmp(name, "2. Medium") == 0) {
                    mvwprintw(sub_menu_win, 9, 4, "Difficulty: MEDIUM");
                    player.game_difficulty = 2;

                } else if (strcmp(name, "3. Hard") == 0) {
                    mvwprintw(sub_menu_win, 9, 4, "Difficulty: HARD");
                    player.game_difficulty = 3;
                }
                wattroff(sub_menu_win, COLOR_PAIR(2));
                wclrtoeol(sub_menu_win);
                wrefresh(sub_menu_win);
            }
                break;
        }
        wrefresh(sub_menu_win);
    }




}
void player_color(MENU *menu, WINDOW *menu_win) {
    clear();
    refresh();
    char *color_choices[] = {
        "1. Red",
        "2. Blue",
        "3. Green",
        "4. Yellow",
        "5. Magenta",
        "6. Back",
        NULL,
    };

    int n_sub_choices = ARRAY_SIZE(color_choices) - 1;
    ITEM **sub_items = (ITEM **)calloc(n_sub_choices + 1, sizeof(ITEM *));
    for (int i = 0; i < n_sub_choices; ++i)
        sub_items[i] = new_item(color_choices[i], NULL);
    sub_items[n_sub_choices] = NULL;

    MENU *sub_menu = new_menu((ITEM **)sub_items);
    // Create submenu window
    WINDOW *sub_menu_win = newwin(12, 40, (LINES - 12) / 2, (COLS - 40) / 2);
    keypad(sub_menu_win, TRUE);

    set_menu_win(sub_menu, sub_menu_win);
    set_menu_sub(sub_menu, derwin(sub_menu_win, 8, 38, 3, 1));
    set_menu_mark(sub_menu, " > ");

    // Print a border and title
    box(sub_menu_win, 0, 0);
    print_in_middle(sub_menu_win, 1, 0, 40, "Choose Player Color", COLOR_PAIR(1));
    mvwaddch(sub_menu_win, 2, 0, ACS_LTEE);
    mvwhline(sub_menu_win, 2, 1, ACS_HLINE, 38);
    mvwaddch(sub_menu_win, 2, 39, ACS_RTEE);
    refresh();

    // Post the menu
    post_menu(sub_menu);
    wrefresh(sub_menu_win);
    refresh();

    int c;
    while ((c = wgetch(sub_menu_win)) != 'q') {
        switch (c) {
            case KEY_DOWN:
                menu_driver(sub_menu, REQ_DOWN_ITEM);
                break;
            case KEY_UP:
                menu_driver(sub_menu, REQ_UP_ITEM);
                break;
            case 10: {  // Enter key
                ITEM *cur = current_item(sub_menu);
                const char *name = item_name(cur);
                wattron(sub_menu_win, COLOR_PAIR(2));


                if (strcmp(name, "6. Back") == 0) {
                    // Cleanup
                    unpost_menu(sub_menu);
                    free_menu(sub_menu);
                    for (int i = 0; i < n_sub_choices; ++i)
                        free_item(sub_items[i]);
                    delwin(sub_menu_win);

                    // Return to main menu
                    clear();
                    refresh();
                    set_menu_win(menu, menu_win);
                    set_menu_sub(menu, derwin(menu_win, 8, 38, 3, 1));
                    set_menu_mark(menu, " > ");

                    // Print a border and title
                    box(menu_win, 0, 0);
                    print_in_middle(menu_win, 1, 0, 40, "Setting", COLOR_PAIR(1));
                    mvwaddch(menu_win, 2, 0, ACS_LTEE);
                    mvwhline(menu_win, 2, 1, ACS_HLINE, 38);
                    mvwaddch(menu_win, 2, 39, ACS_RTEE);

                    // Post the menu
                    post_menu(menu);
                    wrefresh(menu_win);
                    return;
                    // game_setting(menu, menu_win);

                } else if (strcmp(name,"1. Red") == 0) {
                    mvwprintw(sub_menu_win, 10, 5, "Rogue Color: Red");
                    player.player_color = 3;

                } else if (strcmp(name, "2. Blue") == 0) {
                    mvwprintw(sub_menu_win, 10, 5, "Rogue Color: Blue");
                    player.player_color = 5;

                } else if (strcmp(name, "3. Green") == 0) {
                    mvwprintw(sub_menu_win, 10, 5, "Rogue Color: Green");
                    player.player_color = 4;

                }else if (strcmp(name, "4. Yellow") == 0) {
                    mvwprintw(sub_menu_win, 10, 5, "Rogue Color: Yellow");
                    player.player_color = 7;

                }else if (strcmp(name, "5. Magenta") == 0) {
                    mvwprintw(sub_menu_win, 10, 5, "Rogue Color: Magenta");
                    player.player_color = 1;
                }
                wattroff(sub_menu_win, COLOR_PAIR(2));
                wclrtoeol(sub_menu_win);
                wrefresh(sub_menu_win);
            }
                break;
        }
        wrefresh(sub_menu_win);
    }


}
void play_music(const char *filename) {
    Mix_HaltMusic();  // Stop current music

    Mix_Music *music = Mix_LoadMUS(filename);
    if (!music) {
        printf("Failed to load music: %s\n", Mix_GetError());
        endwin();
        exit(0);
        return;
    }

    Mix_PlayMusic(music, -1);  // Play new music in a loop
}
void select_music(MENU *menu, WINDOW *menu_win) {
    clear();
    refresh();
    char *music_choices[] = {
        "1. Stranger Things",
        "2. Sneaky Snitch",
        "3. Pixel Dreams",
        "4. Pixel Fight",
        "5. Music Off",
        "6. Back",
        NULL,
    };

    int n_sub_choices = ARRAY_SIZE(music_choices) - 1;
    ITEM **sub_items = (ITEM **)calloc(n_sub_choices + 1, sizeof(ITEM *));
    for (int i = 0; i < n_sub_choices; ++i)
        sub_items[i] = new_item(music_choices[i], NULL);
    sub_items[n_sub_choices] = NULL;

    MENU *sub_menu = new_menu((ITEM **)sub_items);
    // Create submenu window
    WINDOW *sub_menu_win = newwin(12, 40, (LINES - 12) / 2, (COLS - 40) / 2);
    keypad(sub_menu_win, TRUE);

    set_menu_win(sub_menu, sub_menu_win);
    set_menu_sub(sub_menu, derwin(sub_menu_win, 8, 38, 3, 1));
    set_menu_mark(sub_menu, " > ");

    // Print a border and title
    box(sub_menu_win, 0, 0);
    print_in_middle(sub_menu_win, 1, 0, 40, "Choose Music", COLOR_PAIR(1));
    mvwaddch(sub_menu_win, 2, 0, ACS_LTEE);
    mvwhline(sub_menu_win, 2, 1, ACS_HLINE, 38);
    mvwaddch(sub_menu_win, 2, 39, ACS_RTEE);
    refresh();

    // Post the menu
    post_menu(sub_menu);
    wrefresh(sub_menu_win);
    refresh();

    int c;
    while ((c = wgetch(sub_menu_win)) != 'q') {
        switch (c) {
            case KEY_DOWN:
                menu_driver(sub_menu, REQ_DOWN_ITEM);
                break;
            case KEY_UP:
                menu_driver(sub_menu, REQ_UP_ITEM);
                break;
            case 10: {  // Enter key
                ITEM *cur = current_item(sub_menu);
                const char *name = item_name(cur);
                wattron(sub_menu_win, COLOR_PAIR(2));


                if (strcmp(name, "6. Back") == 0) {
                    // Cleanup
                    unpost_menu(sub_menu);
                    free_menu(sub_menu);
                    for (int i = 0; i < n_sub_choices; ++i)
                        free_item(sub_items[i]);
                    delwin(sub_menu_win);

                    // Return to main menu
                    clear();
                    refresh();
                    set_menu_win(menu, menu_win);
                    set_menu_sub(menu, derwin(menu_win, 8, 38, 3, 1));
                    set_menu_mark(menu, " > ");

                    // Print a border and title
                    box(menu_win, 0, 0);
                    print_in_middle(menu_win, 1, 0, 40, "Setting", COLOR_PAIR(1));
                    mvwaddch(menu_win, 2, 0, ACS_LTEE);
                    mvwhline(menu_win, 2, 1, ACS_HLINE, 38);
                    mvwaddch(menu_win, 2, 39, ACS_RTEE);

                    // Post the menu
                    post_menu(menu);
                    wrefresh(menu_win);
                    return;
                    // game_setting(menu, menu_win);

                } else if (strcmp(name,"1. Stranger Things") == 0) {
                    mvwprintw(sub_menu_win, 10, 4, "Let's Listen to Stranger Things");
                    player.song = 1;
                    play_music("stranger_things.mp3");


                } else if (strcmp(name, "2. Sneaky Snitch") == 0) {
                    mvwprintw(sub_menu_win, 10, 4, "Let's Listen to Sneaky Snitch");
                    player.song = 2;
                    play_music("sneaky_snitch.mp3");

                } else if (strcmp(name, "3. Pixel Dreams") == 0) {
                    mvwprintw(sub_menu_win, 10, 4, "Let's Listen to Pixel Dreams");
                    player.song = 3;
                    play_music("pixel_dreams.mp3");



                }else if (strcmp(name, "4. Pixel Fight") == 0) {
                    mvwprintw(sub_menu_win, 10, 4, "Let's Listen to Pixel Fight");
                    player.song = 4;
                    play_music("pixel_fight.mp3");



                }else if (strcmp(name, "5. Music Off") == 0) {
                    mvwprintw(sub_menu_win, 10, 4, "OK, No Music...");
                    player.song = 5;
                    Mix_HaltMusic();  // Stop current music
                    Mix_CloseAudio();
                    SDL_Quit();
                    // play_music("game_of_thrones.mp3");

                }
                wattroff(sub_menu_win, COLOR_PAIR(2));
                wclrtoeol(sub_menu_win);
                wrefresh(sub_menu_win);
            }
                break;
        }
        wrefresh(sub_menu_win);
    }




}

//Score Board menu
void show_leaderboard(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        mvprintw(LINES / 2, (COLS - 20) / 2, "Error: Cannot open file!");
        refresh();
        getch();
        return;
    }

    // Read users into an array
    Player players[MAX_USERS];
    int count = 0;
    char line[100];


    // Read each line of the file
    while (fgets(line, sizeof(line), file) && count < MAX_USERS) {
        char *username = strtok(line, ":");
        strtok(NULL, ":");  // Skip password
        char *score_str = strtok(NULL, ":");
        char *gold_str = strtok(NULL, ":");
        char *games_str = strtok(NULL, ":");
        char *experience_str = strtok(NULL, ":");

        if (username) {
            // Store player username
            strncpy(players[count].username, username, MAX_NAME_LEN - 1);
            players[count].username[MAX_NAME_LEN - 1] = '\0';  // Ensure null termination

            // Parse and assign default values for missing fields
            players[count].score = (score_str != NULL) ? atoi(score_str) : 0;
            players[count].gold = (gold_str != NULL) ? atoi(gold_str) : 0;
            players[count].count_games = (games_str != NULL) ? atoi(games_str) : 0;
            players[count].experience = (experience_str != NULL) ? atoi(experience_str) : 0;

            count++;  // Move to the next player
        }
    }
    fclose(file);

    // Sort players by score (Descending Order)
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (players[j].score > players[i].score) {
                Player temp = players[i];
                players[i] = players[j];
                players[j] = temp;
            }
        }
    }

    // Create a centered window for the scoreboard
    int win_width = 55, win_height = DISPLAY_ROWS + 20;  // Extra space for headers & footer
    int startx = (COLS - win_width) / 2;
    int starty = (LINES - win_height) / 2;
    WINDOW *score_win = newwin(win_height, win_width, starty, startx);
    keypad(score_win, TRUE);

    box(score_win, 0, 0);
    wattron(score_win, COLOR_PAIR(1));
    mvwprintw(score_win, 1, (win_width - 14) / 2, "Score Board");
    wattroff(score_win, COLOR_PAIR(1));
    wrefresh(score_win);
    mvwaddch(score_win, 2, 0, ACS_LTEE);
    mvwhline(score_win, 2, 1, ACS_HLINE, win_width - 2);
    mvwaddch(score_win, 2, win_width - 1, ACS_RTEE);

    int start_index = 1;  // For scrolling
    int selected = 0;
    int ch;

    while (1) {
        // Clear the scoreboard area before printing
        for (int i = 3; i < win_height - 2; i++) {
            mvwprintw(score_win, i, 2, "%*s", win_width - 4, " "); // Erase old content
        }

        //Display scoarboard header
        mvwprintw(score_win , 3 , 2 , "Rank | Username | Score | Gold | Games | Experience");
        // Display leaderboard inside the window (with scrolling)
        for (int i = 0; i < DISPLAY_ROWS && (start_index + i) < count; i++) {
            int idx = start_index + i;
            if (idx == selected)
                wattron(score_win, A_REVERSE);
            mvwprintw(score_win, 5 + i, 2, "%-18s %4d %4d %4d %4d",
                players[idx].username, players[idx].score, players[idx].gold, players[idx].count_games, players[idx].experience);
            wattroff(score_win, A_REVERSE);
        }

        // Footer instructions
        wattron(score_win, COLOR_PAIR(2));
        mvwprintw(score_win, win_height - 4, 2, "[UP/DOWN] Scroll");
        mvwprintw(score_win, win_height - 3, 2, "[ENTER/Q] Back");
        wattroff(score_win, COLOR_PAIR(2));
        wrefresh(score_win);

        // Handle input
        ch = wgetch(score_win);
        switch (ch) {
            case KEY_UP:
                if (selected > 0) {
                    selected--;
                    if (selected < start_index) {
                        start_index--;
                    }
                }
                break;
            case KEY_DOWN:
                if (selected < count - 1) {
                    selected++;
                    if (selected >= start_index + DISPLAY_ROWS) {
                        start_index++;
                    }
                }
                break;
            case 10:  // Enter key
            case 'q':
                delwin(score_win);
                clear();
                refresh();
                return;
        }
    }
}

#endif