#include <stdlib.h>

#include "supercobrinha.h"
#include "game.h"
#include "snake.h"
#include "draw.h"

typedef struct Food {
	char character;
	int y, x;
	int isonmap;
	int isunique;
	int rarity;
} Food;

Food *newfood(char c, int unique, int rarity) {
	Food *f = malloc(sizeof(Food));
	f->character = c;
	f->isonmap = 0;
	f->isunique = unique;
	f->rarity = rarity;
	return f;
}

void generatefood(WINDOW *w, Food *f) {
	if(f->isunique && f->isonmap) return;
	if((rand() % 10) + 1 < f->rarity) return;

	int valid = 0;
	while(!valid) {
		valid = 1;

		f->y = rand() % (maxiny - 1);
		f->x = rand() % (maxinx - 1);

		if(f->y == maxiny || f->y == 0) valid = 0;
		if(f->x == maxinx || f->x == 0) valid = 0;

		for(int i = 0; i < maxindex + 1; i++) {
			if(f->x == snake[i]->x && f->y == snake[i]->y) valid = 0;
		}
	}
	f->isonmap = 1;
	wattron(inner, COLOR_PAIR(GAMECORES.corFood));
	mvwprintw(inner, f->y, f->x, "%c", f->character);
	wattroff(inner, COLOR_PAIR(GAMECORES.corFood));
}

int checkfoodcolision(Food *f, Snakepart *s) {
	if(s->x == f->x && s->y == f->y) {
		f->isonmap = FALSE;
		return 1;
	}
	return 0;
}
