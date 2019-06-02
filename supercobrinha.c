#include <ncurses.h>
#include <locale.h>
#include <time.h>
#include <stdlib.h>
#include "menus.h"

#define LTR_COLEMAK 1
#define LTR_QWERTY 2

int maxx, maxy;
int middlex, middley;
int ltrup, ltrdwn, ltrrght, ltrlft;
int layout;

void updatesize(void) {
	getmaxyx(stdscr, maxy, maxx);
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
	updatesize();

	setletters(LTR_COLEMAK);
	srand(time(NULL));

	int exit = 0;
	while(!exit) {
		exit = mainmenu();
	}

	endwin();
}
