#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <time.h>
#include <unistd.h>
#include "snake.h"

#define INITIAL_SIZE 4

#define LTR_COLEMAK 1
#define LTR_QWERTY 2

#define MODE_BORDER 1
#define MODE_BORDERLESS 2

#define NORTH 1
#define SOUTH 2
#define EAST 3
#define WEST 4

int maxx, maxy;
int middlex, middley;

int ltrup, ltrdwn, ltrrght, ltrlft;

int layout;

void startgame(int);

void updatesize(void) {
	getmaxyx(stdscr, maxy, maxx);
	middlex = maxx / 2;
	middley = maxy / 2;
}

int centerx(char *c) {
	return (maxx - strlen(c)) / 2;
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
				mvwchgat(w, y + 2 * i, centerx(options[i]), strlen(options[i]), A_STANDOUT, COLOR_RED, NULL);
			} else {
				mvwchgat(w, y + 2 * i, centerx(options[i]), strlen(options[i]), A_NORMAL, COLOR_RED, NULL);
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
			break;
		case 1:
			setletters(LTR_QWERTY);
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

void startgame(int mode) {
	WINDOW *game = newwin(maxy * 0.8, maxx, 0, 0);
	makeborder(game);
	keypad(game, TRUE);
	
	WINDOW *scoreboard = newwin(maxy * 0.2, maxx, maxy * 0.8 + 1, 0);
	makeborder(scoreboard);

	Snakepart *snake[100];
	int maxgamey = 0.8 * maxy;
	int maxgamex = maxx;
	int direction = -1;
	int food = 0;
	int foody, foodx;
	int grow = 0;
	int index = INITIAL_SIZE - 1;

	for(int i = 0; i < INITIAL_SIZE; i++) {
		snake[i] = newpart(i, middley + i, middlex);
		mvwprintw(game, snake[i]->y, snake[i]->x, "O");
	}
	
	wrefresh(game);

	while(direction == -1) {
		const int g = wgetch(game);

		if(g == KEY_UP || g == ' ' || g == '\n' || g == ltrup) {
			direction = NORTH;
		} else if(g == KEY_LEFT || g == ltrlft) {
			direction = WEST;
		} else if(g == KEY_RIGHT || g == ltrrght) {
			direction = EAST;
		}
	}

	nodelay(game, TRUE);

	for(;;) {
		const int g = wgetch(game);
		if(g != ERR) {
			if((g == KEY_UP || g  == ltrup) && direction != SOUTH) {
				direction = NORTH;
			} else if((g == KEY_DOWN || g == ltrdwn) && direction != NORTH) {
				direction = SOUTH;
			} else if((g == KEY_LEFT || g == ltrlft) && direction != EAST) {
				direction = WEST;
			} else if((g == KEY_RIGHT || g == ltrrght) && direction != WEST) {
				direction = EAST;
			}
		}

		if(!food) {
			int valid = 0;
			while(!valid) {
				valid = 1;

				srand(time(NULL));
				foody = rand() % (maxgamey - 1);
				foodx = rand() % (maxgamex - 1);

				if(foody == maxgamey || foody == 0) valid = 0;
				if(foodx == maxgamex || foodx == 0) valid = 0;

				for(int i = 0; i < index + 1; i++) {
					if(foodx == snake[i]->x || foody == snake[i]->y) valid = 0;
				}
			}

			mvwprintw(game, foody, foodx, "x");
			food = 1;
		}

		Snakepart *head = getpartwithindex(snake, index + 1, 0);
		Snakepart *tail = getpartwithindex(snake, index + 1, index);
		
		if(grow) {
			grow = 0;
			index++;
			snake[index] = newpart(index, tail->y, tail->x);
			tail = snake[index];
		} else {
			mvwprintw(game, tail->y, tail->x, " ");
		}


		switch(direction) {
			case NORTH:
				tail->x = head->x;
				tail->y = head->y - 1;
				break;
			case SOUTH:
				tail->x = head->x;
				tail->y = head->y + 1;
				break;
			case EAST:
				tail->x = head->x + 1;
				tail->y = head->y;
				break;
			case WEST:
				tail->x = head->x - 1;
				tail->y = head->y;
		}

		for(int i = 0; i < index + 1; i++) {
			snake[i]->index++;
		}

		tail->index = 0;
		mvwprintw(game, tail->y, tail->x, "O");

		head = tail;
		if(head->x == foodx && head->y == foody) {
			food = 0;
			grow = 1;
		}

		for(int i = 0; i < index + 1; i++) {
			if(snake[i] == head) continue;
			if(snake[i]->x == head->x && snake[i]->y == head->y) {
				return;
			}

			if(mode == MODE_BORDER) {
				if(head->x == maxgamex - 1 || head->x == 0 || head->y == maxgamey - 1 || head->y == 0) {
					return;
				}
			} else if(mode == MODE_BORDERLESS) {
				if(head->x == maxgamex - 1) {
					head->x = 1;
				} else if(head->x == 1) {
					head->x = maxgamex - 2;
				}

				if(head->y == maxgamey - 1) {
					head->y = 1;
				} else if(head->y == 1) {
					head->y = maxgamey - 2;
				}
			}
		}

		wrefresh(game);
		usleep(100 * 1000);
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

	int exit = 0;
	while(!exit) {
		exit = mainmenu();
	}

	endwin();
}
