#ifndef FOOD_HEADER
#define FOOD_HEADER

#include "snake.h"

typedef struct Food {
	char character;
	int isonmap;
	int isunique;
	int rarity;
} Food;

Food *newfood(char c, int unique, int rarity);

void generatefood(WINDOW *w, Food *f);

int checkfoodcolision(Food *f, Snakepart *s);

#endif
