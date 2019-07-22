#include <ncurses.h>
#include <locale.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#include "menus.h"
#include "datamanagement.h"
#include "draw.h"
#include "map.h"
#include "game.h"

#define LTR_QWERTY 0
#define LTR_COLEMAK 1

#define IDLE 0
#define READY 1
#define RUNNING 2
#define PAUSED 3
#define DEATH 4

int GAMESTATE = IDLE;

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
	// mvwprintw(wmain, maxy - 2, 1, "Layout: %s", layout == LTR_COLEMAK ? "Colemak" : "QWERTY ");
	// wrefresh(wmain);
}

void updatestate(int state) {
	GAMESTATE = state;
	draw_state();
}

void intro(void) {
	int py, px;
	int cor;
	int exit = 0;
	nodelay(wmain, TRUE);
	for(int i = 0; i < 30; i++) {
		if(i == 29 || wgetch(wmain) != ERR) {
			py = 1;
			px = (maxx - 72) / 2;
			exit = 1;
			cor = GAMECORES.corTitle;
		} else {
			py = rand() % (maxy-7);
			px = rand() % (maxx-64);
			cor = WHITE + i;
		}
		draw_title(py, px, cor % 5);
		usleep(100000);
		if(exit == 1){
			break;
		}
		wclear(wmain);
	}
	draw_border(wmain);
	nodelay(wmain, FALSE);
}

int main(void) {
	setlocale(LC_ALL, "");
	
	initscr();
	cbreak();
	noecho();
	curs_set(FALSE);
	set_escdelay(0);

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
	gamemap = newmap("default");
	srand(time(NULL));

	loadscheme();
	intro();

	int lay;
	loadoptions(&lay, NULL, NULL, NULL);
	setletters(lay);

	int exit = 0;
	while(!exit) {
		updatestate(IDLE);
		exit = mainmenu();
	}

	endwin();
}
