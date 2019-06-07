#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#include "supercobrinha.h"
#include "menus.h"
#include "snake.h"
#include "food.h"

#define INITIAL_SIZE 4 //tamanho inicial da cobra
#define FOOD_NUM 1 //quantidade de comidas geradas por vez

#define MODE_BORDER 1
#define MODE_BORDERLESS 2
#define MODE_TIMEATK 3

#define NORTH 1
#define SOUTH 2
#define EAST 3
#define WEST 4

Snakepart *snake[100]; //a SuperCobrinha

time_t start; //long int que guarda o tempo atual do computador no momento em que a partida se inicia
int score = 0; //placar
int timerx, timery; //coordenadas de posicionamento do timer
int direction; //direcao atual da cobrinha
int grow; //variavel que diz se a cobrinha deve(1) ou nao(0) aumentar na proxima iteracao
int maxindex; //indice da cauda da cobrinha

Food *foods[FOOD_NUM];

void initialsetup(void) { //setup inicial do jogo
	direction = -1;
	grow = 0;
	maxindex = INITIAL_SIZE - 1;

	wclear(inner);
	makeborder(inner);

	for(int i = 0; i < INITIAL_SIZE; i++) { //cria a cobrinha com tamanho inicial INITIAL_SIZE, INITIAL_SIZE precisa ser menor que 14
		snake[i] = newpart(i, maxiny / 2 + i, maxinx / 2);
		mvwaddch(inner, snake[i]->y, snake[i]->x, ACS_BLOCK);
	}

	foods[0] = newfood('X', TRUE, 0);
	
	wrefresh(inner);

	while(direction == -1) {
		const int g = wgetch(inner); //aguarda input inicial para comecar jogo

		if(g == KEY_UP || g == ' ' || g == '\n' || g == ltrup) { //caso espaco ou enter sejam pressionados, direcao inicial para cima
			direction = NORTH;
		} else if(g == KEY_LEFT || g == ltrlft) {
			direction = WEST;
		} else if(g == KEY_RIGHT || g == ltrrght) {
			direction = EAST;
		}
	}

	nodelay(inner, TRUE);
}

void startgame(int mode, int times, int brd) {
	initialsetup(); //chama o setup inicial
	start = time(NULL); //guarda o tempo atual do computador no instante que o jogo se inicia

	timerx = ((maxx - 32) / 2) - 12; //pega coordenadas to timer, usadas tambem para o score
	timery = 8;

	for(;;) {
		if(mode == MODE_TIMEATK){
			mvwprintw(wmain, timery, timerx, "Tempo: %li", start + times - time(NULL)); //exibe a contagem regressiva no modo MODE_TIMEATK
			if((start+times - time(NULL)) <10)
				mvwprintw(wmain,timery,timerx+8," ");
			if((start+times - time(NULL)) <100)
				mvwprintw(wmain,timery,timerx+9," ");
			wrefresh(wmain);
			if(start+times == time(NULL)){ //morte por tempo limite
				killsnake(snake, maxindex + 1);
				mvwprintw(wmain,15,(maxx - 10) / 2,"Seu tempo acabou");
				while(!wgetch(wmain));
				mvwprintw(wmain,timery,timerx,"          ");
				score = 0;
				mvwprintw(wmain,timery+2,timerx,"          ");
				wrefresh(wmain);
				return;
			}
		}

		mvwprintw(wmain, timery+2, timerx,"Score: %d", score); //exibe o placar atual
		wrefresh(wmain);
		
		const int g = wgetch(inner); //pega a direcao 
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

		for(int i = 0; i < FOOD_NUM; i++) { //cria comida
			generatefood(inner, foods[i]);
		}

		Snakepart *head = getpartwithindex(snake, maxindex + 1, 0);
		Snakepart *tail = getpartwithindex(snake, maxindex + 1, maxindex);
		
		if(grow) { //aumenta a cobra
			maxindex++;
			snake[maxindex] = newpart(maxindex, tail->y, tail->x);
			tail = snake[maxindex];
			grow = 0;
		} else {
			mvwprintw(inner, tail->y, tail->x, " ");
		}

		switch(direction) { // !!!!! meio confuso isso aqui !!!!!
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
		head = tail; //tira elemento da cauda e poe na cabeca

		for(int i = 0; i < FOOD_NUM; i++) { //verifica colisao com a comida
			if(checkfoodcolision(foods[i], head)) {
				grow = 1;
				score++;
			}
		}

		for(int i = 0; i < maxindex + 1; i++) {
			if(snake[i] == head) continue;
			if(snake[i]->x == head->x && snake[i]->y == head->y) { //morte por colisao consigo mesmo
				killsnake(snake, maxindex + 1);
				mvwprintw(wmain,15,(maxx - 10) / 2,"voce faleceu");
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

		if(mode == MODE_BORDER || brd == 1) {
			if(head->x == maxinx - 1 || head->x == 0 || head->y == maxiny - 1 || head->y == 0) { //morte por colisao na parede
				killsnake(snake, maxindex + 1);
				mvwprintw(wmain,15,(maxx - 10) / 2,"voce faleceu");
				while(!wgetch(wmain));
				mvwprintw(wmain,timery+2,timerx,"          ");
				score = 0;
				if(mode == MODE_TIMEATK){
					mvwprintw(wmain,timery,timerx,"          ");
				}
				wrefresh(wmain);
				return;
			}
		} else if(mode == MODE_BORDERLESS || brd == 0) { //teleporte do modo sem bordas
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
		
		mvwaddch(inner, head->y, head->x, ACS_BLOCK); //imprime a cabeca da cobra na tela

		wrefresh(inner);
		usleep(200 * 1000); //tempo que a cobrinha leva para se mover, em microsegundos
	}
}
