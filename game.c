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

#define BORDER 1
#define BORDERLESS 2

#define MODE_CLASSIC 1
#define MODE_TIMEATK 2

#define NORTH 1
#define SOUTH 2
#define EAST 3
#define WEST 4

Snakepart *snake[100];

time_t start;
time_t gametime;
int score = 0;
int timerx, timery;
int direction;
int grow;
int maxindex;

Food *foods[FOOD_NUM];

void initialsetup(void) {
	// Inicializacao de variaveis
	direction = -1;
	grow = 0;
	maxindex = INITIAL_SIZE - 1;

	// coordenadas do timer e do placar
	timerx = ((maxx - 32) / 2) - 14;
	timery = 8;
	
	// Inicializao de todas as comidas
	foods[0] = newfood('o', TRUE, 0);

	// Desenho do estado inicial do campo de jogo
	wclear(inner);
	makeborder(inner);

	for(int i = 0; i < INITIAL_SIZE; i++) {
		snake[i] = newpart(i, maxiny / 2 + i, maxinx / 2);
		mvwaddch(inner, snake[i]->y, snake[i]->x, ACS_BLOCK);
	}

	wrefresh(inner);

	// Espera o jogador fazer o movimento inicial
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

void deathclear() {
	mvwprintw(wmain,timery,timerx,"            ");
	score = 0;
	mvwprintw(wmain,timery+2,timerx,"          ");
	wrefresh(wmain);
}

void startgame(int mode, int border, int times) {
	initialsetup();

	//tempo precisa ser inicializado aqui
	start = time(NULL);

	// Loop principal
	for(;;) {
		mvwprintw(wmain, timery+2, timerx,"Score: %d", score);
		wrefresh(wmain);

		// Atualiza o tempo de jogo
		if (mode == MODE_TIMEATK) {
			gametime = start + times - time(NULL);
		} else {
			gametime = time(NULL) - start;
		}


		// Atualiza a direcao da cobrinha
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
				// softpause quando enter for pressionado
				nodelay(inner, FALSE);
				while(wgetch(inner) != '\n');
				if (mode == MODE_TIMEATK){
					start = time(NULL) - (times - gametime);
				} else {
					start = time(NULL) - gametime;
				}
				nodelay(inner, TRUE);
			}
		}

		// Tenta gerar todas as comidas
		for(int i = 0; i < FOOD_NUM; i++) {
			generatefood(inner, foods[i]);
		}

		Snakepart *head = getpartwithindex(snake, maxindex + 1, 0);
		Snakepart *tail = getpartwithindex(snake, maxindex + 1, maxindex);
		
		// Verifica se a cobrinha deve crescer
		if(grow) {
			maxindex++;
			snake[maxindex] = newpart(maxindex, tail->y, tail->x);
			tail = snake[maxindex];
			grow = 0;
		} else {
			mvwprintw(inner, tail->y, tail->x, " ");
		}

		// Movimenta a cobrinha
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

		// Verifica colisao com cada tipo de comida
		for(int i = 0; i < FOOD_NUM; i++) {
			if(checkfoodcolision(foods[i], head)) {
				grow = 1;
				score++;
			}
		}

		// Verifica colisao com a propria cobrinha
		for(int i = 0; i < maxindex + 1; i++) {
			if(snake[i] == head) continue;
			if(snake[i]->x == head->x && snake[i]->y == head->y) {
				killsnake(snake, maxindex + 1);
				mvwprintw(wmain,15,(maxx - 10) / 2,"voce faleceu");
				wgetch(wmain);
				deathclear();
				return;
			}
		}

		// Verifica colisao com borda
		if(border == BORDER) {
			if(head->x == maxinx - 1 || head->x == 0 || head->y == maxiny - 1 || head->y == 0) {
				killsnake(snake, maxindex + 1);
				mvwprintw(wmain,15,(maxx - 10) / 2,"voce faleceu");
				wgetch(wmain);
				deathclear();
				return;
			}

		// Faz a cobra "dar a volta"
		} else if(border == BORDERLESS) {
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
		
		// Exibe o tempo de jogo
		mvwprintw(wmain, timery, timerx, "Tempo:  %li:%li", gametime/60, gametime%60);
		if(gametime % 60 <10){
			mvwprintw(wmain,timery,timerx+10,"0%li", gametime%60);
		}
		if(gametime / 60 <10){
			mvwprintw(wmain,timery,timerx+7,"0");
		}
		wrefresh(wmain);

		// modo Time attack
		if (mode == MODE_TIMEATK) {
			if(start + times <= time(NULL)){
				killsnake(snake, maxindex + 1);
				mvwprintw(wmain,15,(maxx - 10) / 2,"Seu tempo acabou");
				wgetch(wmain);
				deathclear();
				return;
			}
		}

		
		mvwaddch(inner, head->y, head->x, ACS_BLOCK);

		wrefresh(inner);
		usleep(200 * 1000);
	}
}
