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

void draw_food(Food *f) {
    if(f->onmap == 1) {
        return;
    }

	int foodx, foody;
    int valid = 0;
	while(!valid) {
		valid = 1;

		foody = rand() % (maxiny - 1);
		foodx = rand() % (maxinx - 1);

		if(foody == maxiny || foody == 0) {
            valid = 0;
		}
		if(foodx == maxinx || foodx == 0) {
            valid = 0;
        }

		for(int i = 0; i < maxindex + 1; i++) {
			if(foodx == snake[i]->x && foody == snake[i]->y) {
                valid = 0;
			}
		}
	}

	f->onde.y = foody;
	f->onde.x = foodx;
    wattron(inner, COLOR_PAIR(GAMECORES.corFood));
	mvwprintw(inner, foody, foodx, "%c", f->caracter);
	wattroff(inner, COLOR_PAIR(GAMECORES.corFood));
	f->onmap = 1;
}

int checkfoodcolision(Food *f) {
	if(Active.x + 1 == f->onde.x && Active.y + 1 == f->onde.y) {
		f->onmap = 0;
		return 1;
	}
	return 0;
}
