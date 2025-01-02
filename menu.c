#include <ncurses.h>
#include <menu.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define MAX_LENGTH 100
// Menu choices
char *choices[] = {
        "Create New Rogue",
        "Rogue Login",
        "Before You Play",
        "Score Board",
        "Setting",
        "Exit",
        NULL,
};

void print_in_middle(WINDOW *win, int starty, int startx, int width, char *string, chtype color);

void create_new_rogue(MENU *menu, WINDOW *menu_win, chtype color);
bool username_exists(const char *filename, const char *username);
void register_user(const char *filename, const char *username, const char *password);
bool contains_number(const char *password);
bool contains_lowercase(const char *password);
bool contains_uppercase(const char *password);
bool is_valid_email(const char *email);
char *generate_random_password();





int main() {
    ITEM **items;
    MENU *menu;
    WINDOW *menu_win;
    int n_choices, c, i;

    // Initialize curses
    initscr();
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    init_pair(1, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(2, COLOR_CYAN, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_BLACK);

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
    while ((c = wgetch(menu_win)) != 'q') {
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
                if (strcmp(item_name(cur), "Exit") == 0) {
                    goto end;  // Exit the menu loop
                } else if (strcmp(item_name(cur), "Create New Rogue") == 0) {
                    create_new_rogue(menu, menu_win, COLOR_PAIR(2));  // Pass the menu and menu_win
                }
            }
                break;

        }
        box(menu_win, 0, 0);
        print_in_middle(menu_win, 1, 0, win_width, "Game Menu", COLOR_PAIR(1));
        mvwaddch(menu_win, 2, 0, ACS_LTEE);
        mvwhline(menu_win, 2, 1, ACS_HLINE, win_width - 2);
        mvwaddch(menu_win, 2, win_width - 1, ACS_RTEE);

        // Post the menu
        post_menu(menu);
        wrefresh(menu_win);
    }

    end:
    // Cleanup
    unpost_menu(menu);
    free_menu(menu);
    for (i = 0; i < n_choices; ++i)
        free_item(items[i]);
    endwin();

    return 0;
}

// Function to print text in the middle of a window
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

// Function to handle the "Create New Rogue" option
void create_new_rogue(MENU *menu, WINDOW *menu_win, chtype color) {
    clear();
    WINDOW *form_win = newwin(14, 50, (LINES - 12) / 2, (COLS - 50) / 2);
    box(form_win, 0, 0);
    wattron(form_win, COLOR_PAIR(2));
    mvwprintw(form_win, 1, 16, "Create New Rogue");
    wattroff(form_win, COLOR_PAIR(2));
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

    }
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
    }
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
    }

    noecho();

    // Check and store user data
//    const char *filename = "users.txt";
    if (username_exists(filename, username)) {
        wattron(form_win, COLOR_PAIR(1));
        mvwprintw(form_win, 9, 3, "Username already exists! Try again.");
        wattroff(form_win, COLOR_PAIR(1));
    } else {
        register_user(filename, username, password);
        wattron(form_win, COLOR_PAIR(2));
        mvwprintw(form_win, 9, 3, "User registered successfully!");
        wattroff(form_win, COLOR_PAIR(2));
    }
    wattron(form_win, COLOR_PAIR(2));
    mvwprintw(form_win, 11, 3, "Press any key to return to the menu...");
    wattroff(form_win, COLOR_PAIR(2));

    wrefresh(form_win);
    wgetch(form_win);

    delwin(form_win);

    // Clear and redisplay the menu
    clear();
    refresh();
    post_menu(menu);
    wrefresh(menu_win);
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

    fprintf(file, "%s:%s\n", username, password);  // Store username and password
    fclose(file);
//    printf("User '%s' registered successfully!\n", username);
}

