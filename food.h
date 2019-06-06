#ifndef FOOD_HEADER
#define FOOD_HEADER

typedef struct Food {
	char character;
	int isonmap;
	int isunique;
	int rarity;
} Food;

Food *newfood(char c, int unique, int rarity);

void generatefood(WINDOW *w, Food *f);

#endif
