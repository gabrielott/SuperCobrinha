#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#include "supercobrinha.h"
#include "menus.h"
#include "snake.h"
#include "food.h"

#define INITIAL_SIZE 4
#define FOOD_NUM 1

#define MODE_BORDER 1
#define MODE_BORDERLESS 2
#define MODE_TIMEATK 3

#define NORTH 1
#define SOUTH 2
#define EAST 3
#define WEST 4

Snakepart *snake[100];

time_t start;
int score = 0;
int timerx, timery;
int direction;
int grow;
int maxindex;

Food *foods[FOOD_NUM];

void initialsetup(void) {
	direction = -1;
	grow = 0;
	maxindex = INITIAL_SIZE - 1;

	wclear(inner);
	makeborder(inner);

	for(int i = 0; i < INITIAL_SIZE; i++) {
		snake[i] = newpart(i, maxiny / 2 + i, maxinx / 2);
		mvwprintw(inner, snake[i]->y, snake[i]->x, "0");
	}

	foods[0] = newfood('X', TRUE, 0);
	
	wrefresh(inner);

	while(direction == -1) {
		const int g = wgetch(inner);

		if(g == KEY_UP || g == ' ' || g == '\n' || g == ltrup) {
			direction = NORTH;
		} else if(g == KEY_LEFT || g == ltrlft) {
			direction = WEST;
		} else if(g == KEY_RIGHT || g == ltrrght) {
			direction = EAST;
		}
	}

	nodelay(inner, TRUE);
}

void startgame(int mode, int times, int bord) {
	initialsetup();
	start = time(NULL);
	timerx = ((COLS - 32) / 2) - 12;
	timery = 8;
	for(;;) {
		if(mode == MODE_TIMEATK){
			mvwprintw(wmain, timery, timerx, "Tempo: %li", start + times - time(NULL));
			if((start+times - time(NULL)) <10)
				mvwprintw(wmain,timery,timerx+8," ");
			if((start+times - time(NULL)) <100)
				mvwprintw(wmain,timery,timerx+9," ");
			wrefresh(wmain);
			if(start+times == time(NULL)){
				killsnake(snake, maxindex + 1);
				mvwprintw(wmain,15,(COLS - 10) / 2,"Tempo Acabou");
				while(!wgetch(wmain));
				mvwprintw(wmain,timery,timerx,"          ");
				score = 0;
				mvwprintw(wmain,timery+2,timerx,"          ");
				wrefresh(wmain);
				return;
			}
		}
		mvwprintw(wmain, timery+2, timerx,"Score: %d", score);
		wrefresh(wmain);
		
		const int g = wgetch(inner);
		if(g != ERR) {
			if((g == KEY_UP || g  == ltrup) && direction != SOUTH) {
				direction = NORTH;
			} else if((g == KEY_DOWN || g == ltrdwn) && direction != NORTH) {
				direction = SOUTH;
			} else if((g == KEY_LEFT || g == ltrlft) && direction != EAST) {
				direction = WEST;
			} else if((g == KEY_RIGHT || g == ltrrght) && direction != WEST) {
				direction = EAST;
			} else if(g == '\n') {
				nodelay(inner, FALSE);
				while(wgetch(inner) != '\n');
				nodelay(inner, TRUE);
			}
		}

		for(int i = 0; i < FOOD_NUM; i++) {
			generatefood(inner, foods[i]);
		}

		Snakepart *head = getpartwithindex(snake, maxindex + 1, 0);
		Snakepart *tail = getpartwithindex(snake, maxindex + 1, maxindex);
		
		if(grow) {
			maxindex++;
			snake[maxindex] = newpart(maxindex, tail->y, tail->x);
			tail = snake[maxindex];
			grow = 0;
		} else {
			mvwprintw(inner, tail->y, tail->x, " ");
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

		for(int i = 0; i < FOOD_NUM; i++) {
			if(checkfoodcolision(foods[i], head)) {
				grow = 1;
				score++;
			}
		}

		for(int i = 0; i < maxindex + 1; i++) {
			if(snake[i] == head) continue;
			if(snake[i]->x == head->x && snake[i]->y == head->y) {
				killsnake(snake, maxindex + 1);
				mvwprintw(wmain,15,(COLS - 10) / 2,"voce faleceu");
				while(!wgetch(wmain));
				score = 0;
				mvwprintw(wmain,timery+2,timerx,"          ");
				if(mode == MODE_TIMEATK){
					mvwprintw(wmain,timery,timerx,"          ");
				}
				wrefresh(wmain);
				return;
			}
		}

		if(mode == MODE_BORDER || bord == 1) {
			if(head->x == maxinx - 1 || head->x == 0 || head->y == maxiny - 1 || head->y == 0) {
				killsnake(snake, maxindex + 1);
				mvwprintw(wmain,15,(COLS - 10) / 2,"voce faleceu");
				while(!wgetch(wmain));
				mvwprintw(wmain,timery+2,timerx,"          ");
				score = 0;
				if(mode == MODE_TIMEATK){
					mvwprintw(wmain,timery,timerx,"          ");
				}
				wrefresh(wmain);
				return;
			}
		} else if(mode == MODE_BORDERLESS || bord == 0) {
			if(head->x == maxinx - 1) {
				head->x = 1;
			} else if(head->x == 0) {
				head->x = maxinx - 2;
			}

			if(head->y == maxiny - 1) {
				head->y = 1;
			} else if(head->y == 0) {
				head->y = maxiny - 2;
			}
		}
		
		mvwprintw(inner, head->y, head->x, "O");

		wrefresh(inner);
		usleep(200 * 1000);
	}
}
