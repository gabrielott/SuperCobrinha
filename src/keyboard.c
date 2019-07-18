#include <ncurses.h>
#include <stdio.h>
#include <stdarg.h>

#include "supercobrinha.h"
#include "datamanagement.h"
#include "draw.h"

#define NORTH 1
#define SOUTH 2
#define EAST 3
#define WEST 4

int xmody(int x, int y) {
	if(x >= 0) {
		return x % y;
	}
	while(x < 0) {
		x += y;
	}
	return x;
}

int key_pressed(int keyvar, int quant, ...) {
	va_list press;
	va_start(press, quant);
	for(int i = 0; i < quant; i++) {
		if(keyvar == va_arg(press, int)) {
			va_end(press);
			return 1;
		}
	}
	va_end(press);
	return 0;
}

/*Objetivo:
typedef struct comandos {
	// Ponteiros para funcoes, so que nao sei seus tipos entao nao eh bem isso que eu quero ainda
	void (*UP)(int);
	void (*DOWN)(int);
	void (*LEFT)(int);
	void (*RIGHT)(int);
	void (*SPACE)(int);
	void (*ESC)(int);
} comandos;

comandos PRESS;

void nulo() { // Quando a tecla nao faz nada, eh porque ela esta apontando para essa funcao
	return;
}

void set_keys() {
	if(GAMESTATE == IDLE) {
		PRESS.UP = nulo;
		PRESS.DOWN = nulo;
	} else if() {
	
	}
}

void key_command(int g){
	if(key_pressed(g, 2, KEY_UP, ltrup)) {
		PRESS.UP(KEY_UP);
	}
}
*/

int key_command(int g, int direction) {

	if(GAMESTATE == READY) {
		if(key_pressed(g, 4, KEY_UP, ' ', '\n', ltrup)) {
			return NORTH;
		} else if(key_pressed(g, 2, KEY_LEFT, ltrlft)) {
			return WEST;
		} else if(key_pressed(g, 2, KEY_RIGHT, ltrrght)) {
			return EAST;
		}
		// Caso o usuario nao pressione algo valido
		return -1;
	}

	if(GAMESTATE == RUNNING) {
		if(key_pressed(g, 2, KEY_UP, ltrup) && direction != SOUTH) {
			return NORTH;
		} else if(key_pressed(g, 2, KEY_DOWN, ltrdwn) && direction != NORTH) {
			return SOUTH;
		} else if(key_pressed(g, 2, KEY_LEFT, ltrlft) && direction != EAST) {
			return WEST;
		} else if(key_pressed(g, 2, KEY_RIGHT, ltrrght) && direction != WEST) {
			return EAST;
		}
		// Caso nada tenha sido pressionado, retorna a mesma direcao que ja estava antes
		return direction;
	}

	// Caso ocorra algum erro
	return 42;
}

int menu_command_1(int g, int selec, int optamt, int *choose) { // Usada na makeselector
	if(g == KEY_UP || g == ltrup) {
		return xmody(selec - 1, optamt);
	} else if(g == KEY_DOWN || g == ltrdwn) {
		return xmody(selec + 1, optamt);
	} else if(g == ' ' || g == '\n') {
		*choose = 1;
	}
	return selec;
}

int menu_command_2(int g, int *map, int *gtime) { // Usada no menu do scoreboard
	if(g == KEY_RIGHT || g == ltrrght) {
		*map = xmody(*map + 1, 2);
	} else if(g == KEY_LEFT || g == ltrlft) {
		*map = xmody(*map - 1, 2);
	} else if(g == KEY_UP || g == ltrup) {
		*gtime = xmody(*gtime - 1, 5);
	} else if(g == KEY_DOWN || g == ltrdwn) {
		*gtime = xmody(*gtime + 1, 5);
	} else if(g == ' ' || g == '\n') {
		return 1;
	}
	return 0;
}

int menu_command_3(int g, int selected, int current[], int opt_amt, int amt_index[], int *goback) { // Usada no menu de opcoes
	if(selected != opt_amt - 1) {
		mvwprintw(inner, 3 + 2*selected, 16, "          ");
	}
	// Condicao para se mover no menu para cima e para baixo
	if(g == KEY_UP || g == ltrup) {
		return xmody(selected - 1, opt_amt);
	} else if(g == KEY_DOWN || g == ltrdwn) {
		return xmody(selected + 1, opt_amt);
	} 
	// Condicao para alterar as opcoes do menu para os lados (configurar como substates)
	else if((g == KEY_LEFT || g == ltrlft) && selected != opt_amt - 1) {
		current[selected] = xmody(current[selected] - 1, amt_index[selected]);
		// Casos especiais
		if(selected == 0) {
			setletters(current[selected]);
		} else if(selected == 4) {
			GAMECORES = setscheme(current[selected]);
			redraw_all();
		}
	} else if((g == KEY_RIGHT || g == ltrrght) && selected != opt_amt - 1) {
		current[selected] = xmody(current[selected] + 1, amt_index[selected]);
		if(selected == 0) {
			setletters(current[selected]);
		} else if(selected == 4) {
			GAMECORES = setscheme(current[selected]);
			redraw_all();
		}
	}
	// Condicao para salvar as opcoes e sair do menu
	else if((g == ' ' || g == '\n') && selected == opt_amt - 1) {
		saveoptions(current[0], current[1], current[2], current[3]);
		savescheme();
		*goback = 1;
	}
	return selected;
}