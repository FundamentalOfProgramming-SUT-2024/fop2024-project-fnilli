#ifndef THEMES_H
#define THEMES_H

#include "essentials.h"
#include "game_menu.h"


void place_gold( Room room[MAX_ROOMS]);
void get_gold(char map[MAX_ROW][COLS]);
void place_dark_gold( Room room[MAX_ROOMS]);
void get_dark_gold(char map[MAX_ROW][COLS]);



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
	    if (which_room == player.stair_pos) {
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
}

void place_dark_gold( Room room[MAX_ROOMS]) {
	int which_room = rand() % 6;
	// Ensure the treasure is not placed in the same room as stairs
	if (which_room == player.stair_pos) {
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
}



#endif //THEMES_H
