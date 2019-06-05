#include <ncurses.h>
#include <string.h>

#include "supercobrinha.h"
#include "game.h"
#include "datamanagement.h"

int centerx(WINDOW *w, char *c) {
	int y, x;
	getmaxyx(w, y, x);
	return (maxx - strlen(c)) / 2;
}

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

int makeselector(WINDOW *w, int y, int optamt, char *options[]) {
	for(int i = 0; i < optamt; i++) {
		mvwprintw(w, y + 2 * i, (maxx - strlen(options[i])) / 2, options[i]);
	}

	int selected = 0;

	for(;;) {
		for(int i = 0; i < optamt; i++) {
			if(selected == i) {
				mvwchgat(w, y + 2 * i, centerx(w, options[i]), strlen(options[i]), A_STANDOUT, COLOR_RED, NULL);
			} else {
				mvwchgat(w, y + 2 * i, centerx(w, options[i]), strlen(options[i]), A_NORMAL, COLOR_RED, NULL);
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
	WINDOW *w = newwin(maxy, maxx, 0, 0);
	keypad(w, TRUE);

	makeborder(w);

	char *options[] = {"Usar layout Colemak", "Usar layout QWERTY", "Cancelar"};

	switch(makeselector(w, maxy * 0.2, 3, options)) {
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
	WINDOW *w = newwin(maxy, maxx, 0, 0);
	keypad(w, TRUE);

	makeborder(w);

	int py = maxy * 0.2;
	const int px = (maxx - 72) / 2;

	mvwprintw(w, py++, px, "   _____                                  __         _       __         ");
	mvwprintw(w, py++, px, "  / ___/__  ______  ___  ______________  / /_  _____(_)___  / /_  ____ _");
	mvwprintw(w, py++, px, "  \\__ \\/ / / / __ \\/ _ \\/ ___/ ___/ __ \\/ __ \\/ ___/ / __ \\/ __ \\/ __ `/");
	mvwprintw(w, py++, px, " ___/ / /_/ / /_/ /  __/ /  / /__/ /_/ / /_/ / /  / / / / / / / / /_/ / ");
	mvwprintw(w, py++, px, "/____/\\__,_/ .___/\\___/_/   \\___/\\____/_.___/_/  /_/_/ /_/_/ /_/\\__,_/  ");
	mvwprintw(w, py, px, "          /_/                                                           ");

	mvwprintw(w, maxy - 2, 1, "Layout: %s", layout == LTR_COLEMAK ? "Colemak" : "QWERTY");

	char *options[] = {"Sem bordas", "Com bordas", "Opções", "Sair"};

	switch(makeselector(w, maxy * 0.4, 4, options)) {
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
