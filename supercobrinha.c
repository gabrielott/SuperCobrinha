#include <ncurses.h>
#include <locale.h>
#include <time.h>
#include <stdlib.h>

#include "menus.h"
#include "datamanagement.h"

#define LTR_COLEMAK 1
#define LTR_QWERTY 2

int maxy, maxx;
int maxiny, maxinx;
int middlex, middley;
int ltrup, ltrdwn, ltrrght, ltrlft;
int layout;

WINDOW *wmain, *inner;

void updatesize(void) {
	getmaxyx(wmain, maxy, maxx);
	getmaxyx(inner, maxiny, maxinx);
	middlex = maxx / 2;
	middley = maxy / 2;
}

void setletters(int l) {
	if(l == LTR_COLEMAK) {
		layout = LTR_COLEMAK;
		ltrup = 'w';
		ltrdwn = 'r';
		ltrlft = 'a';
		ltrrght = 's';
	} else if(l == LTR_QWERTY) {
		layout = LTR_QWERTY;
		ltrup = 'w';
		ltrdwn = 's';
		ltrlft = 'a';
		ltrrght = 'd';
	}
}

int main(void) {
	setlocale(LC_ALL, "");
	
	initscr();
	cbreak();
	noecho();
	curs_set(FALSE);

	wmain = newwin(LINES, COLS, 0, 0);
	inner = newwin(16, 32, (LINES - 16) / 2, (COLS - 32) / 2);

	keypad(inner, TRUE);
	updatesize();
	setupsaves();
	srand(time(NULL));

	int py = maxy * 0.1;
	const int px = (maxx - 72) / 2;

	makeborder(wmain);
	mvwprintw(wmain, py++, px, "   _____                                  __         _       __         ");
	mvwprintw(wmain, py++, px, "  / ___/__  ______  ___  ______________  / /_  _____(_)___  / /_  ____ _");
	mvwprintw(wmain, py++, px, "  \\__ \\/ / / / __ \\/ _ \\/ ___/ ___/ __ \\/ __ \\/ ___/ / __ \\/ __ \\/ __ `/");
	mvwprintw(wmain, py++, px, " ___/ / /_/ / /_/ /  __/ /  / /__/ /_/ / /_/ / /  / / / / / / / / /_/ / ");
	mvwprintw(wmain, py++, px, "/____/\\__,_/ .___/\\___/_/   \\___/\\____/_.___/_/  /_/_/ /_/_/ /_/\\__,_/  ");
	mvwprintw(wmain, py, px, "          /_/                                                           ");
	wrefresh(wmain);

	if(loadoptions() == LTR_COLEMAK) {
		setletters(LTR_COLEMAK);
	} else {
		setletters(LTR_QWERTY);
	}

	int exit = 0;
	while(!exit) {
		exit = mainmenu();
	}

	endwin();
}
