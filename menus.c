#include <ncurses.h>
#include <string.h>

#include "supercobrinha.h"
#include "game.h"
#include "datamanagement.h"

void makeborder(WINDOW *w) {
	int x, y;
	getmaxyx(w, y, x);

	mvwprintw(w, 0, 0, "+");
	mvwprintw(w, 0, x - 1, "+");
	mvwprintw(w, y - 1, 0, "+");
	mvwprintw(w, y - 1, x - 1, "+");

	for(int i = 1; i < x - 1; i++) {
		mvwprintw(w, 0, i, "=");
		mvwprintw(w, y - 1, i, "=");
	}

	for(int i = 1; i < y - 1; i++) {
		mvwprintw(w, i, 0, "|");
		mvwprintw(w, i, x - 1, "|");
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
		} else if((g == KEY_DOWN || g == ltrdwn) && selected < optamt - 1) {
			selected++;
		} else if(g == ' ' || g == '\n') {
			return selected;
		}
	}
}

void optionsmenu(void) {
	wclear(inner);
	makeborder(inner);

	char *options[] = {"Usar layout Colemak", "Usar layout QWERTY", "Cancelar"};

	switch(makeselector(inner, 3, options)) {
		case 0:
			setletters(LTR_COLEMAK);
			saveoptions(LTR_COLEMAK);
			break;
		case 1:
			setletters(LTR_QWERTY);
			saveoptions(LTR_QWERTY);
			break;
		case 2:
			return;
	}
}

int mainmenu(void) {
	mvwprintw(wmain, maxy - 2, 1, "Layout: %s", layout == LTR_COLEMAK ? "Colemak" : "QWERTY");
	char *options[] = {"Sem bordas", "Com bordas", "Opções", "Sair"};

	wclear(inner);
	makeborder(inner);
	switch(makeselector(inner, 4, options)) {
		case 0:
			startgame(MODE_BORDERLESS);
			return 0;
		case 1:
			startgame(MODE_BORDER);
			return 0;
		case 2:
			optionsmenu();
			return 0;
		case 3:
			return 1;
		default:
			return 0;
	}
}
