#include <ncurses.h>
#include <locale.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#include "menus.h"
#include "datamanagement.h"

#define LTR_QWERTY 0
#define LTR_COLEMAK 1

#define WHITE 0
#define GREEN 1
#define YELLOW 2
#define RED 3
#define CYAN 4

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
	mvwprintw(wmain, maxy - 2, 1, "Layout: %s", layout == LTR_COLEMAK ? "Colemak" : "QWERTY ");
	wrefresh(wmain);
}

void updatestate(int state) {
	GAMESTATE = state;
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

void intro(void) {
	int py, px;
	int exit = 0;
	nodelay(wmain, TRUE);
	for(int i = 0; i < 30; i++) {
		if(i == 29 || wgetch(wmain) != ERR) {
			py = 1;
			px = (maxx - 72) / 2;
			exit = 1;
			i = 3;
		} else {
			py = rand() % (maxy-7);
			px = rand() % (maxx-64);
		}
		wattron(wmain, COLOR_PAIR(WHITE + i % 5));
		mvwprintw(wmain, py, px, "   _____                                  __         _       __         ");
		mvwprintw(wmain, py+1, px, "  / ___/__  ______  ___  ______________  / /_  _____(_)___  / /_  ____ _");
		mvwprintw(wmain, py+2, px, "  \\__ \\/ / / / __ \\/ _ \\/ ___/ ___/ __ \\/ __ \\/ ___/ / __ \\/ __ \\/ __ `/");
		mvwprintw(wmain, py+3, px, " ___/ / /_/ / /_/ /  __/ /  / /__/ /_/ / /_/ / /  / / / / / / / / /_/ / ");
		mvwprintw(wmain, py+4, px, "/____/\\__,_/ .___/\\___/_/   \\___/\\____/_.___/_/  /_/_/ /_/_/ /_/\\__,_/  ");
		mvwprintw(wmain, py+5, px, "          /_/                                                           ");
		wattroff(wmain, COLOR_PAIR(WHITE + i % 5));
		wrefresh(wmain);
		usleep(100000);
		if(exit == 1){
			break;
		}
		wclear(wmain);
	}
	makeborder(wmain);
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
	srand(time(NULL));

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
