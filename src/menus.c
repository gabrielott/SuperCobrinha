#include <ncurses.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "supercobrinha.h"
#include "game.h"
#include "datamanagement.h"

void makeborder(WINDOW *w) {
	int x, y;
	getmaxyx(w, y, x);

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
}

int makeselector(WINDOW *w, int optamt, char *options[]) {
	int y, x;
	getmaxyx(w, y, x);

	wrefresh(w);

	for(int i = 0; i < optamt; i++) {
		mvwprintw(w, (y - optamt * 2) / 2 + i * 2, (x - strlen(options[i])) / 2, options[i]);
	}

	int selected = 0;

	for(;;) {
		for(int i = 0; i < optamt; i++) { 
			if(selected == i) {
				mvwchgat(w, (y - optamt * 2) / 2 + i * 2, (x - strlen(options[i])) / 2, strlen(options[i]), A_STANDOUT, COLOR_RED, NULL);
			} else {
				mvwchgat(w, (y - optamt * 2) / 2 + i * 2, (x - strlen(options[i])) / 2, strlen(options[i]), A_NORMAL, COLOR_RED, NULL);
			}
		}

		wrefresh(w);

		int g = wgetch(w);
		if((g == KEY_UP || g == ltrup) && selected > 0) {
			selected--;
		} else if((g == KEY_UP || g == ltrup) && selected == 0) {
			selected = optamt - 1;
		} else if((g == KEY_DOWN || g == ltrdwn) && selected < optamt - 1) {
			selected++;
		} else if((g == KEY_DOWN || g == ltrdwn) && selected == optamt - 1) {
			selected = 0;
		} else if(g == ' ' || g == '\n') {
			return selected;
		}
	}
}

int timeatkmenu(int border) {
	wclear(inner);
	makeborder(inner);

	char *options[] = {"00:30", "01:00", "03:00", "05:00", "Voltar"};

	int time;
	switch(makeselector(inner, 5, options)) {
		case 0:
			time = 30;
			break;
		case 1:
			time = 60;
			break;
		case 2:
			time = 180;
			break;
		case 3:
			time = 300;
			break;
		case 4:
			time = 0;
			return 1;
	}
	startgame(MODE_TIMEATK, border, time);
	return 0;
}

int bordermenu(void) {
	wclear(inner);
	makeborder(inner);

	char *options[] = {"Borda", "Sem Borda", "Voltar"};

	switch(makeselector(inner, 3, options)) {
		case 0:
			return BORDER;
		case 1:
			return BORDERLESS;
		case 2:
			return 0;
		default:
			return 0;
	}
}

void credits(void) {
	wclear(inner);
	makeborder(inner);

	wrefresh(inner);
	nodelay(inner, TRUE);

	char *nomes[] = {"Filipe Castelo", "Gabriel Ottoboni", "João Pedro", "Rodrigo Delpreti"};
	int i, n = -1;
	for (i=14*4-1;i>=0;i--){
		if(wgetch(inner) != ERR){
			return;
		}
		if(((i+1)% 14) == 0){
			n++;
		}
		mvwprintw(inner,1+(i%14),(maxinx - strlen(nomes[n])) / 2,nomes[n]);
		mvwprintw(inner,1+((i+1)%14),(maxinx - strlen("                         ")) / 2,"                         ");
		wrefresh(inner);
		nodelay(inner, FALSE);
		usleep(500000);
		nodelay(inner, TRUE);
	}
	return;
}

void optionsmenu(void) {
	int exit = 0;
	while(!exit) {
		wclear(inner);
		makeborder(inner);

		char *options[] = {"Usar layout Colemak", "Usar layout QWERTY", "Creditos", "Cancelar"};

		exit = 1;
		switch(makeselector(inner, 4, options)) {
			case 0:
				setletters(LTR_COLEMAK);
				saveoptions(LTR_COLEMAK);
				break;
			case 1:
				setletters(LTR_QWERTY);
				saveoptions(LTR_QWERTY);
				break;
			case 2:
				credits();
				exit = 0;
				break;
			case 3:
				break;
		}
	}
}

int mainmenu(void) {
	wclear(inner);
	makeborder(inner);

	char *options[] = {"Clássico", "Time Attack", "Opções", "Sair"};

	int ans = makeselector(inner, 4, options);
	int border;

	if(ans == 3) return 1;
	if(ans != 2) {
		border = bordermenu();
		if(border == 0) return 0;
	}

	switch(ans) {
		case 0:
			startgame(MODE_CLASSIC, border, 0);
			return 0;
		case 1:
			while(timeatkmenu(border)) {
				border = bordermenu();
				if(border == 0) break;
			}
			return 0;
		case 2:
			optionsmenu();
			return 0;
		default:
			return 0;
	}
	return 0;
}
