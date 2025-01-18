#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#include "game_menu.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define MAX_LENGTH 100
char *choices[] = {
        "1. Create New Rogue",
        "2. Rogue Login",
        "3. Before You Play",
        "4. Exit",
        NULL,
};

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
                if (strcmp(item_name(cur), "4. Exit") == 0) {
                    goto end;  // Exit the menu loop
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
