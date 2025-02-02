#ifndef THEMES_H
#define THEMES_H

#include "essentials.h"
#include "game_menu.h"


void place_gold( Room room[MAX_ROOMS]);
void get_gold(char map[MAX_ROW][COLS]);
void place_dark_gold( Room room[MAX_ROOMS]);
void get_dark_gold(char map[MAX_ROW][COLS]);

void place_trap( Room room[MAX_ROOMS]);

void place_food( Room room[MAX_ROOMS]);
void get_food(char map[MAX_ROW][COLS]);



void update_user_gold(const char *filename, const char *username, int score);


void place_gold( Room room[MAX_ROOMS]) {
  	int gold_count_per_floor = 0;
    switch(player.game_difficulty){
        case 1:		gold_count_per_floor = 3; break;
		case 2:		gold_count_per_floor = 2; break;
        case 3:		gold_count_per_floor = 1; break;
	}
	for(int i = 0; i < gold_count_per_floor; i++){
		int which_room = rand() % 6;
    	// Ensure the treasure is not placed in the same room as stairs
	    if (which_room == player.forward_stair_pos) {
    	    which_room = (which_room + 1) % 6;
	    }

    	int y = room[which_room].y + rand_range(1,room[which_room].height - 2) ;
	    int x = room[which_room].x + rand_range(1, room[which_room].width - 2) ;

	    if ((mvinch(y, x) & A_CHARTEXT) != '.') {
	        // If not, recursively try again
    	    place_gold(room);
        	room[which_room].type = 3;	//treasure room
	        return;
	    }
    	mvaddch(y, x, '$');
		// map[y][x] = '$';
    }

}
void get_gold(char map[MAX_ROW][COLS]){

	int y = player.player_pos.y;
    int x = player.player_pos.x;

    //pick the gold and clear map
    if (map[y][x] != '$') {
        if (y > 0 && map[y-1][x] == '$') y--;
        else if (y < MAX_ROW - 1 && map[y+1][x] == '$') y++;
        else if (x > 0 && map[y][x-1] == '$') x--;
        else if (x < COLS - 1 && map[y][x+1] == '$') x++;
        else return;  // No valid placement found
    }
	map[y][x] = '.';
    mvaddch(y,x, '.');

    //add gold score
    player.gold += 5;
	update_user_gold("users.txt", player.username, 5);
}
void place_dark_gold( Room room[MAX_ROOMS]) {
	int which_room = rand() % 6;
	// Ensure the treasure is not placed in the same room as stairs
	if (which_room == player.forward_stair_pos) {
		which_room = (which_room + 1) % 6;
	}

	int y = room[which_room].y + rand_range(1,room[which_room].height - 2) ;
	int x = room[which_room].x + rand_range(1, room[which_room].width - 2) ;

	if ((mvinch(y, x) & A_CHARTEXT) != '.') {
		// If not, recursively try again
		place_dark_gold(room);
		room[which_room].type = 3;	//treasure room
		return;
	}
	mvaddch(y, x, '"');
	// map[y][x] = '$';
}
void get_dark_gold(char map[MAX_ROW][COLS]){

	int y = player.player_pos.y;
	int x = player.player_pos.x;

	//pick the gold and clear map
	if (map[y][x] != '"') {
		if (y > 0 && map[y-1][x] == '"') y--;
		else if (y < MAX_ROW - 1 && map[y+1][x] == '"') y++;
		else if (x > 0 && map[y][x-1] == '"') x--;
		else if (x < COLS - 1 && map[y][x+1] == '"') x++;
		else return;  // No valid placement found
	}
	map[y][x] = '.';
	mvaddch(y,x, '.');

	//add gold score
	player.gold += 10;
	update_user_gold("users.txt", player.username, 10);

}

void update_user_gold(const char *filename, const char *username, int gold) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return;
    }

    char temp_filename[] = "temp_users.txt";
    FILE *temp_file = fopen(temp_filename, "w");
    if (!temp_file) {
        perror("Error opening temp file");
        fclose(file);
        return;
    }

    char line[MAX_LINE];
    int found = 0;  // Flag to check if user is found

    while (fgets(line, sizeof(line), file)) {
        char file_user[MAX_USERNAME], file_pass[MAX_USERNAME];
        int file_score, file_gold=0, file_game_count, file_experience;

        // Parse the full format: user:pass:score:gold:game_count:experience
        if (sscanf(line, "%s:%s:%d:%d:%d:%d",
                   file_user, file_pass, &file_score, &file_gold, &file_game_count, &file_experience) == 6) {

            // If the user matches, update their gold
            if (strcmp(file_user, username) == 0) {
                found = 1;
                file_gold += gold;  // Add new gold to existing gold
                fprintf(temp_file, "%s:%s:%d:%d:%d:%d\n",
                        file_user, file_pass, file_score, file_gold, file_game_count, file_experience);
            } else {
                // Write unchanged data for other users
                fprintf(temp_file, "%s", line);
            }
        } else {
            // If the line is not properly formatted, just write it back unchanged
            fprintf(temp_file, "%s", line);
        }
    }

    fclose(file);
    fclose(temp_file);

    // Replace old file with updated temp file
    remove(filename);
    rename(temp_filename, filename);

    // // Display result
    // if (!found && !player.guest) {
    //     mvprintw(0, 50, "User %s not found in %s", username, filename);
    // } else {
    //     mvprintw(0, 50, "User %s gold updated successfully.", username);
    // }
    refresh();
}

void place_food( Room room[MAX_ROOMS]) {	//&
	int food_count_per_floor = 0;
	switch(player.game_difficulty){
		case 1:		food_count_per_floor = 5; break;
		case 2:		food_count_per_floor = 3; break;
		case 3:		food_count_per_floor = 2; break;
	}
	for(int i = 0; i < food_count_per_floor; i++){
		int which_room = rand() % 6;
		// Ensure the treasure is not placed in the same room as stairs
		if (which_room == player.first_room_pos) {
			which_room = (which_room + 1) % 6;
		}

		int y = room[which_room].y + rand_range(1,room[which_room].height - 2) ;
		int x = room[which_room].x + rand_range(1, room[which_room].width - 2) ;

		if ((mvinch(y, x) & A_CHARTEXT) != '.') {
			// If not, recursively try again
			place_food(room);
			// room[which_room].type = 3;	//treasure room
			return;
		}
		mvaddch(y, x, '&');
	}

}
void get_food(char map[MAX_ROW][COLS]){

	int y = player.player_pos.y;
	int x = player.player_pos.x;

	//pick the gold and clear map
	if (map[y][x] != '&') {
		if (y > 0 && map[y-1][x] == '&') y--;
		else if (y < MAX_ROW - 1 && map[y+1][x] == '&') y++;
		else if (x > 0 && map[y][x-1] == '&') x--;
		else if (x < COLS - 1 && map[y][x+1] == '&') x++;
		else return;  // No valid placement found
	}
	map[y][x] = '.';
	mvaddch(y,x, '.');

	//add health
	if (player.health + 10 > 100)	player.health = 100;
	else	player.health += 10;
	if (player.food_count < 5)		player.food_count++;
}


#endif //THEMES_H
