#include <ncurses.h>
#include <json.h>
#include <stdio.h>
#include <string.h>

// Function to get input from the user
void get_input(WINDOW *win, int y, int x, char *buffer, int buffer_size, int is_password) {
    int ch, i = 0;
    mvwprintw(win, y, x, "> ");
    wrefresh(win);
    while ((ch = wgetch(win)) != '\n' && i < buffer_size - 1) {
        if (ch == KEY_BACKSPACE || ch == 127) {  // Handle backspace
            if (i > 0) {
                i--;
                mvwaddch(win, y, x + 2 + i, ' ');
                wmove(win, y, x + 2 + i);
            }
        } else {
            buffer[i++] = ch;
            if (is_password) {
                mvwaddch(win, y, x + 2 + i - 1, '*');
            } else {
                mvwaddch(win, y, x + 2 + i - 1, ch);
            }
        }
        wrefresh(win);
    }
    buffer[i] = '\0';
}

// Function to save user data to JSON file
void save_user_data(const char *username, const char *password, const char *email) {
    struct json_object *user = json_object_new_object();
    json_object_object_add(user, "username", json_object_new_string(username));
    json_object_object_add(user, "password", json_object_new_string(password));
    json_object_object_add(user, "email", json_object_new_string(email));

    FILE *file = fopen("users.json", "w");
    if (file) {
        fputs(json_object_to_json_string(user), file);
        fclose(file);
    }
    json_object_put(user);  // Free memory
}

// Function to load and check user data
int check_user_data(const char *username, const char *password) {
    FILE *file = fopen("users.json", "r");
    if (!file) {
        return 0;  // No file found
    }

    char buffer[1024];
    fread(buffer, sizeof(char), 1024, file);
    fclose(file);

    struct json_object *user = json_tokener_parse(buffer);
    const char *saved_username = json_object_get_string(json_object_object_get(user, "username"));
    const char *saved_password = json_object_get_string(json_object_object_get(user, "password"));

    int valid = strcmp(username, saved_username) == 0 && strcmp(password, saved_password) == 0;
    json_object_put(user);  // Free memory
    return valid;
}

int main() {
    initscr();
    noecho();
    cbreak();

    WINDOW *win = newwin(10, 50, 5, 10);
    box(win, 0, 0);
    mvwprintw(win, 1, 2, "Enter your details:");

    char username[50], password[50], email[50];
    get_input(win, 2, 2, username, sizeof(username), 0);
    get_input(win, 3, 2, password, sizeof(password), 1);
    get_input(win, 4, 2, email, sizeof(email), 0);

    save_user_data(username, password, email);

    mvwprintw(win, 6, 2, "Saved! Now checking...");
    wrefresh(win);

    if (check_user_data(username, password)) {
        mvwprintw(win, 7, 2, "User validated successfully!");
    } else {
        mvwprintw(win, 7, 2, "Invalid credentials!");
    }
    wrefresh(win);

    wgetch(win);
    endwin();

    return 0;
}
