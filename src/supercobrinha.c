#include <ncurses.h>
#include <locale.h>
#include <time.h>
#include <stdlib.h>

#include "menus.h"
#include "datamanagement.h"

#define LTR_COLEMAK 1
#define LTR_QWERTY 2

#define WHITE 1
#define GREEN 2
#define YELLOW 3
#define RED 4
#define CYAN 5

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
	mvwprintw(wmain, maxy - 2, 1, "Layout: %s", layout == LTR_COLEMAK ? "Colemak" : "QWERTY ");
	wrefresh(wmain);
}

int main(void) {
	setlocale(LC_ALL, "");
	
	initscr();
	cbreak();
	noecho();
	curs_set(FALSE);

	start_color();
	init_pair(WHITE, COLOR_WHITE, COLOR_BLACK);
	init_pair(GREEN, COLOR_GREEN, COLOR_BLACK);
	init_pair(YELLOW, COLOR_YELLOW, COLOR_BLACK);
	init_pair(RED, COLOR_RED, COLOR_BLACK);
	init_pair(CYAN, COLOR_CYAN, COLOR_BLACK);

	wmain = newwin(LINES, COLS, 0, 0);
	inner = newwin(16, 32, 7, (COLS - 32) / 2);

	updatesize();

	if(maxy < 28 || maxx < 85) {
		mvwprintw(wmain, middley - 2, (maxx - 25) / 2, "Favor ampliar o terminal,");
		mvwprintw(wmain, middley - 1, (maxx - 25) / 2, "pressione algo para sair.");
		while(!wgetch(wmain));
		endwin();
		return 0;
	}

	keypad(inner, TRUE);
	setupsaves();
	srand(time(NULL));

	int py = 1;
	const int px = (maxx - 72) / 2;

	makeborder(wmain);
	wattron(wmain, COLOR_PAIR(CYAN));
	mvwprintw(wmain, py++, px, "   _____                                  __         _       __         ");
	mvwprintw(wmain, py++, px, "  / ___/__  ______  ___  ______________  / /_  _____(_)___  / /_  ____ _");
	mvwprintw(wmain, py++, px, "  \\__ \\/ / / / __ \\/ _ \\/ ___/ ___/ __ \\/ __ \\/ ___/ / __ \\/ __ \\/ __ `/");
	mvwprintw(wmain, py++, px, " ___/ / /_/ / /_/ /  __/ /  / /__/ /_/ / /_/ / /  / / / / / / / / /_/ / ");
	mvwprintw(wmain, py++, px, "/____/\\__,_/ .___/\\___/_/   \\___/\\____/_.___/_/  /_/_/ /_/_/ /_/\\__,_/  ");
	mvwprintw(wmain, py, px, "          /_/                                                           ");
	wattroff(wmain, COLOR_PAIR(CYAN));
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
