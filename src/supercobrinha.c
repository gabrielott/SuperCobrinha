#include <ncurses.h>
#include <locale.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#include "menus.h"
#include "datamanagement.h"

#define LTR_QWERTY 0
#define LTR_COLEMAK 1

#define WHITE 1
#define GREEN 2
#define YELLOW 3
#define RED 4
#define CYAN 5

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
	char *welcome = "Bem vindo ao jogo SuperCobrinha!";
	wattron(wmain, COLOR_PAIR(RED));
	mvwprintw(wmain, middley - 2, (maxx - strlenunicode(welcome)) / 2, welcome);
	wattroff(wmain, COLOR_PAIR(RED));
	wrefresh(wmain);
	while(wgetch(inner) == ERR);
	wclear(wmain);
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

	int py = 1;
	const int px = (maxx - 72) / 2;

	makeborder(wmain);
	wattron(wmain, COLOR_PAIR(RED));
	mvwprintw(wmain, py++, px, "   _____                                  __         _       __         ");
	mvwprintw(wmain, py++, px, "  / ___/__  ______  ___  ______________  / /_  _____(_)___  / /_  ____ _");
	mvwprintw(wmain, py++, px, "  \\__ \\/ / / / __ \\/ _ \\/ ___/ ___/ __ \\/ __ \\/ ___/ / __ \\/ __ \\/ __ `/");
	mvwprintw(wmain, py++, px, " ___/ / /_/ / /_/ /  __/ /  / /__/ /_/ / /_/ / /  / / / / / / / / /_/ / ");
	mvwprintw(wmain, py++, px, "/____/\\__,_/ .___/\\___/_/   \\___/\\____/_.___/_/  /_/_/ /_/_/ /_/\\__,_/  ");
	mvwprintw(wmain, py, px, "          /_/                                                           ");
	wattroff(wmain, COLOR_PAIR(RED));
	wrefresh(wmain);

	int lay;
	loadoptions(&lay, NULL, NULL);
	setletters(lay);

	int exit = 0;
	while(!exit) {
		updatestate(IDLE);
		exit = mainmenu();
	}

	endwin();
}
