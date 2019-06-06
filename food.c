#include "supercobrinha.h"
#include "game.h"

typedef struct Food {
	char character;
	int isonmap;
	int isunique;
	int rarity;
} Food;

Food *newfood(char c, int unique, int rarity) {
	Food f;
	f.character = c;
	f.isonmap = 0;
	f.isunique = unique;
	f.rarity = rarity;
	return &f;
}

void generatefood(WINDOW *w, Food *f) {
	if(f->isunique && f->isonmap) return;
	if((rand() % 10) + 1 < f->rarity) return;

	int valid = 0;
	while(!valid) {
		valid = 1;

		foody = rand() % (maxiny - 1);
		foodx = rand() % (maxinx - 1);

		if(foody == maxiny || foody == 0) valid = 0;
		if(foodx == maxinx || foodx == 0) valid = 0;

		for(int i = 0; i < maxindex + 1; i++) {
			if(foodx == snake[i]->x && foody == snake[i]->y) valid = 0;
		}
	}
	f->isonmap = 1;
	mvwprintw(inner, foody, foodx, "%c", f->character);
}
