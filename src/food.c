#include <stdlib.h>

#include "supercobrinha.h"
#include "game.h"
#include "snake.h"
#include "draw.h"
#include "keyboard.h"

typedef struct Food {
	char caracter;
	coord onde;
	int onmap;
	int rarity;
} Food;

Food newfood(char c, int rare) {
	Food f;
	f.caracter = c;
	f.onmap = 0;
	f.rarity = rare;
	return f;
}

int draw_food(Food f){
    if(f.onmap == 1){
        return 1;
    }

    int valid = 0;
	while(!valid) {
		valid = 1;

		f.onde.y = rand() % (maxiny - 1);
		f.onde.x = rand() % (maxinx - 1);

		if(f.onde.y == maxiny || f.onde.y == 0) {
            valid = 0;
		}
		if(f.onde.x == maxinx || f.onde.x == 0) {
            valid = 0;
        }

		for(int i = 0; i < maxindex + 1; i++) {
			if(f.onde.x == snake[i]->x && f.onde.y == snake[i]->y) {
                valid = 0;
			}
		}
	}

    wattron(inner, COLOR_PAIR(GAMECORES.corFood));
	mvwprintw(inner, f.onde.y, f.onde.x, "%c", f.caracter);
	wattroff(inner, COLOR_PAIR(GAMECORES.corFood));

	return 1;
}

int checkfoodcolision(Food f) {
	if(Active.x == f.onde.x && Active.y == f.onde.y) {
		return 0;
	}
	return 1;
}
