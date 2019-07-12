#include <stdio.h>
#include <ncurses.h>
#include <unistd.h>

#include "supercobrinha.h"
#include "menus.h"
#include "snake.h"

#define WHITE 0
#define GREEN 1
#define YELLOW 2
#define RED 3
#define CYAN 4

void draw_title(int py, int px, int color) {
	wattron(wmain, COLOR_PAIR(color));
	mvwprintw(wmain, py, px, "   _____                                  __         _       __         ");
	mvwprintw(wmain, py+1, px, "  / ___/__  ______  ___  ______________  / /_  _____(_)___  / /_  ____ _");
	mvwprintw(wmain, py+2, px, "  \\__ \\/ / / / __ \\/ _ \\/ ___/ ___/ __ \\/ __ \\/ ___/ / __ \\/ __ \\/ __ `/");
	mvwprintw(wmain, py+3, px, " ___/ / /_/ / /_/ /  __/ /  / /__/ /_/ / /_/ / /  / / / / / / / / /_/ / ");
	mvwprintw(wmain, py+4, px, "/____/\\__,_/ .___/\\___/_/   \\___/\\____/_.___/_/  /_/_/ /_/_/ /_/\\__,_/  ");
	mvwprintw(wmain, py+5, px, "          /_/                                                           ");
	wattroff(wmain, COLOR_PAIR(color));
	wrefresh(wmain);
}

void draw_border(WINDOW *w) {
	int x, y;
	getmaxyx(w, y, x);

	wattron(w, COLOR_PAIR(GREEN));
	mvwaddch(w, 0, 0, ACS_ULCORNER);
	mvwaddch(w, 0, x - 1, ACS_URCORNER);
	mvwaddch(w, y - 1, 0, ACS_LLCORNER);
	mvwaddch(w, y - 1, x - 1, ACS_LRCORNER);

	for(int i = 1; i < x - 1; i++) {
		mvwaddch(w, 0, i, ACS_HLINE);
		mvwaddch(w, y - 1, i, ACS_HLINE);
	}

	for(int i = 1; i < y - 1; i++) {
		mvwaddch(w, i, 0, ACS_VLINE);
		mvwaddch(w, i, x - 1, ACS_VLINE);
	}
	wattroff(w, COLOR_PAIR(GREEN));
}

void draw_state(void) {
	if (GAMESTATE == IDLE){
		mvwprintw(wmain, maxy - 2, maxx - 20, "STATE: Idle   ");
	}
	if (GAMESTATE == READY){
		mvwprintw(wmain, maxy - 2, maxx - 20, "STATE: Ready   ");
	}
	if (GAMESTATE == RUNNING){
		mvwprintw(wmain, maxy - 2, maxx - 20, "STATE: Running");
	}
	if (GAMESTATE == PAUSED){
		mvwprintw(wmain, maxy - 2, maxx - 20, "STATE: Paused ");
	}
	if (GAMESTATE == DEATH){
		mvwprintw(wmain, maxy - 2, maxx - 20, "STATE: Death  ");
	}	
	wrefresh(wmain);
}

void draw_credits(void) {
	nodelay(inner, TRUE);

	char *nomes[] = {"SUPERCOBRINHA", "Desenvolvido por:", "Filipe Castelo", "Gabriel Ottoboni", "João Pedro Silva", "Rodrigo Delpreti", "Obrigado por jogar!"};
	int startl[7] = {-7, 1, 5, 8, 11, 14, 25};
	int i, j;
	int setup = 14*5-1;

	for (i = setup; i >= 0; i--) {
		// Prepara a janela para os creditos serem exibidos
		wclear(inner);
		draw_border(inner);
		wrefresh(inner);

		// Sai dos creditos se qualquer tecla for pressionada
		if(wgetch(inner) != ERR) {
			return;
		}

		// Printa os creditos quando for a vez deles
		for (j=0; j<7; j++) {
			if (j != 6 && i < (setup - startl[j]) && i > (setup - 15 - startl[j])) {
				mvwprintw(inner, 1+((i+1+startl[j])%14), (maxinx - strlenunicode(nomes[j])) / 2, nomes[j]);
			} 
			if (j == 6 && i < (setup - startl[j])) {
				if (i <= setup - 33) {
					mvwprintw(inner, 1+((setup-6)%14), (maxinx - strlenunicode(nomes[j])) / 2, nomes[j]);
				} else {
					mvwprintw(inner, 1+((i+1+startl[j])%14), (maxinx - strlenunicode(nomes[j])) / 2, nomes[j]);
				}
			}
		}
		wrefresh(inner);

		usleep(450000);
	}
	return;
}

void draw_part(Snakepart *part, int color) {
	wattron(inner, COLOR_PAIR(color));
	mvwaddch(inner, part->y, part->x, ACS_BLOCK);
	wattroff(inner, COLOR_PAIR(color));
}