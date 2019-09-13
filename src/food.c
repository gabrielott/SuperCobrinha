#include <stdlib.h>
#include <stdarg.h>

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
	int size;
} Food;

Food newfood(char c, int rare, int fat) {
	Food f;
	f.caracter = c;
	f.onmap = 0;
	f.rarity = rare;
	f.size = fat;
	return f;
}

void draw_food(Food *f) {
    if(f->onmap == 1) {
        return;
    }

	if((rand() % 100) % f->rarity != 0){
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

int checkfoodcolision(int quant, ...) {
	va_list comidas;
	va_start(comidas, quant);
	
	int r = 0;

	for(int i = 0; i < quant; i++) {
		Food *f = va_arg(comidas, Food*);
		if(Active.x + 1 == f->onde.x && Active.y + 1 == f->onde.y) {
			f->onmap = 0;
			r = f->size;
			score += r;
			//break; //deixei comentado para que o tempo gasto em cada loop seja igual
		}
	}
	va_end(comidas);
	return r;
}
