#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>

#include "supercobrinha.h"
#include "menus.h"
#include "snake.h"

#define INITIAL_SIZE 4

#define MODE_BORDER 1
#define MODE_BORDERLESS 2

#define NORTH 1
#define SOUTH 2
#define EAST 3
#define WEST 4

WINDOW *game;
WINDOW *scoreboard;

Snakepart *snake[100];

int maxgamey;
int maxgamex;
int direction;
int food;
int foody, foodx;
int grow;
int maxindex;

void initialsetup(void) {
	game = newwin(maxy * 0.8, maxx, 0, 0);
	makeborder(game);
	keypad(game, TRUE);

	scoreboard = newwin(maxy * 0.2, maxx, maxy * 0.8 + 1, 0);
	makeborder(scoreboard);

	maxgamey = 0.8 * maxy;
	maxgamex = maxx;
	direction = -1;
	food = 0;
	grow = 0;
	maxindex = INITIAL_SIZE - 1;

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
}

void startgame(int mode) {
	initialsetup();

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

				foody = rand() % (maxgamey - 1);
				foodx = rand() % (maxgamex - 1);

				if(foody == maxgamey || foody == 0) valid = 0;
				if(foodx == maxgamex || foodx == 0) valid = 0;

				for(int i = 0; i < maxindex + 1; i++) {
					if(foodx == snake[i]->x && foody == snake[i]->y) valid = 0;
				}
			}

			mvwprintw(game, foody, foodx, "x");
			food = 1;
		}

		Snakepart *head = getpartwithindex(snake, maxindex + 1, 0);
		Snakepart *tail = getpartwithindex(snake, maxindex + 1, maxindex);
		
		if(grow) {
			maxindex++;
			snake[maxindex] = newpart(maxindex, tail->y, tail->x);
			tail = snake[maxindex];
			grow = 0;
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

		for(int i = 0; i < maxindex + 1; i++) {
			snake[i]->index++;
		}

		tail->index = 0;
		head = tail;

		if(head->x == foodx && head->y == foody) {
			food = 0;
			grow = 1;
		}

		for(int i = 0; i < maxindex + 1; i++) {
			if(snake[i] == head) continue;
			if(snake[i]->x == head->x && snake[i]->y == head->y) {
				killsnake(snake, maxindex + 1);
				return;
			}
		}

		if(mode == MODE_BORDER) {
			if(head->x == maxgamex - 1 || head->x == 0 || head->y == maxgamey - 1 || head->y == 0) {
				killsnake(snake, maxindex + 1);
				return;
			}
		} else if(mode == MODE_BORDERLESS) {
			if(head->x == maxgamex - 1) {
				head->x = 1;
			} else if(head->x == 0) {
				head->x = maxgamex - 2;
			}

			if(head->y == maxgamey - 1) {
				head->y = 1;
			} else if(head->y == 0) {
				head->y = maxgamey - 2;
			}
		}
		
		mvwprintw(game, head->y, head->x, "O");

		wrefresh(game);
		usleep(50 * 1000);
	}
}
