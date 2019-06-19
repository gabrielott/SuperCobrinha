#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
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
#define maxque 2

Snakepart *snake[30*14];

time_t start;
time_t gametime;
int score;
int timerx, timery;
int direction;
int grow;
int maxindex;
int g = 0, cont = 0;
int fila[maxque + 1];
int gamespeed = 1;

Food *foods[FOOD_NUM];
void getxtoy(int x, int x2, int *v){
	for(int i =x; i<x2; i++){
		
	}
}
void initialsetup(void) {
	// Inicializacao de variaveis
	direction = -1;
	grow = 0;
	score = 0;
	maxindex = INITIAL_SIZE - 1;

	// Inicializacao da queue
	for (int i = 0; i <= maxque; i++) {
		fila[i] = 0;
	}
	g = 0;

	// coordenadas do timer e do placar
	timerx = ((maxx - 32) / 2) - 14;
	timery = 8;
	
	// Inicializao de todas as comidas
	foods[0] = newfood('o', TRUE, 0);
	//foods[1] = newfood('!', TRUE, 7);

	// Desenho do estado inicial do campo de jogo
	wclear(inner);
	makeborder(inner);

	for(int i = 0; i < INITIAL_SIZE; i++) {
		snake[i] = newpart(i, 11 + i, maxinx / 2);
		mvwaddch(inner, snake[i]->y, snake[i]->x, ACS_BLOCK);
	}

	wrefresh(inner);

	// Espera o jogador fazer o movimento inicial
	while(direction == -1) {
		const int g2 = wgetch(inner);

		if(g2 == KEY_UP || g2 == ' ' || g2 == '\n' || g2 == ltrup) {
			direction = NORTH;
		} else if(g2 == KEY_LEFT || g2 == ltrlft) {
			direction = WEST;
		} else if(g2 == KEY_RIGHT || g2 == ltrrght) {
			direction = EAST;
		}
	}

	nodelay(inner, TRUE);
}

void block(int y, int x, int y2, int x2){
	if(y2 == 0){
		for(int i = x; i < x2; i++){
			mvwaddch(inner, y, i, ACS_HLINE);

		}
	}
	if(x2 == 0){
		for(int i = y; i < y2; i++){
			mvwaddch(inner, i, x, ACS_VLINE);
		}

	}
}
void deathclear(int deathmode) {
	char *mensagem[] = {"Voce perdeu", "O tempo acabou"};
	mvwprintw(inner, 3, (maxinx - strlen(mensagem[deathmode])) / 2, mensagem[deathmode]);
	wrefresh(inner);
	killsnake(snake, maxindex + 1);
	while(wgetch(inner) == ERR);
}

int startgame(int mode, int border, int times, int level) {
	initialsetup();

	// Tempo a partir do qual a partida comeca
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

		// Correcao de bug de multiplos inputs
		// Adiciona caracter pressionado na fila de execucao
		while ((fila[cont] = wgetch(inner)) != ERR) {
            // Pause quando enter ou a barra de espaco forem pressionados
            if(fila[cont] == '\n' || fila[cont] == ' ') {
            	mvwprintw(wmain, timery+4, timerx, "Jogo Pausado");
            	wrefresh(wmain);
			    while(wgetch(inner) != '\n' && wgetch(inner) != ' ');
                // Zera a fila para evitar que a cobra se mova apos o pause
                for (int i = 0; i <= maxque; i++) {
		            fila[i] = 0;
	            }
	            cont = 0;
                // Ajusta timer
			    if (mode == MODE_TIMEATK) {
				    start = time(NULL) - (times - gametime);
			    } else {
				    start = time(NULL) - gametime;
			    }
			    // Limpa o status de jogo pausado
			    mvwprintw(wmain, timery+4, timerx, "            ");
            	wrefresh(wmain);
            }
            // Avanca o indice da fila
			else if (cont < maxque) {
			    cont++;
			}
		}

		// Executa input conforme a ordem da fila de execucao
		if (fila[0] != 0 && fila[0] != ERR){
			// Captura o primeiro da fila para execucao
			g = fila[0];
			// Anda com a fila
			for (int i = 0; i < maxque; i++){
				fila[i] = fila[i+1];
			}
			// Esvazia o final da fila
			fila[maxque] = 0;
			if (cont > 0){	
				cont--;
			}
		}
		switch(level){
			case 1:
				block(4, 6, 6, 0);
				mvwaddch(inner, 3, 6, ACS_ULCORNER);
				block(3, 7, 0, 11);
				block(10, 6, 12, 0);
				mvwaddch(inner, 12, 6, ACS_LLCORNER);
				block(12, 7, 0, 11);
				block(4, 26, 6, 0);
				mvwaddch(inner, 3, 26, ACS_URCORNER);
				block(3, 22, 0, 26);
				block(10, 26, 12, 0);
				mvwaddch(inner, 12, 26, ACS_LRCORNER);
				block(12, 22, 0, 26);
				break;
			case 2:
				block(4, 6, 6, 0);
				mvwaddch(inner, 3, 6, ACS_ULCORNER);
				block(3, 7, 0, 11);
				block(10, 6, 12, 0);
				mvwaddch(inner, 12, 6, ACS_LLCORNER);
				block(12, 7, 0, 11);
				block(4, 26, 6, 0);
				mvwaddch(inner, 3, 26, ACS_URCORNER);
				block(3, 22, 0, 26);
				block(10, 26, 12, 0);
				mvwaddch(inner, 12, 26, ACS_LRCORNER);
				block(12, 22, 0, 26);
				mvwaddch(inner, 8, 16, ACS_PLUS);
				block(8, 12, 0, 16);
				block(8, 17, 0, 21);
				block(6, 16, 8, 0);
				block(9, 16, 11, 0);
				break;	
			case 3:
				block(5, 6, 11, 0);
				block(5, 25, 11, 0);
				block(3, 10, 0, 22);
				block(12, 10, 0, 22);
				block(7, 13, 9, 0);
				block(7, 18, 9, 0);
				break;
			case 4:
				block(3, 1, 0, 14);
				block(3, 20, 0, 31);
				block(6, 1, 0, 8);
				block(6, 14, 0, 31);
				block(9, 1, 0, 6);
				block(9, 12, 0, 31);
				block(12, 1, 0, 16);
				block(12, 22, 0, 31);
				break;
			case 5:
				block(3, 7, 0, 11);
				mvwaddch(inner, 3, 6, ACS_ULCORNER);
				block(4, 6, 7, 0);
				mvwaddch(inner, 7, 6, ACS_LLCORNER);
				block(7, 7, 0, 11);
				block(3, 20, 0, 24);
				mvwaddch(inner, 3, 19, ACS_ULCORNER);
				block(4, 19, 7, 0);
				mvwaddch(inner, 7, 19, ACS_LLCORNER);
				block(7, 20, 0, 24);
				block(12, 8, 0, 24);
				mvwaddch(inner, 12, 7, ACS_LLCORNER);
				mvwaddch(inner, 12, 24, ACS_LRCORNER);
				block(10, 7, 12, 0);
				block(10, 24, 12, 0);
				break;

			default:
				break;
		} 

		// Atualiza a direcao da cobrinha
		if((g == KEY_UP || g  == ltrup) && direction != SOUTH) {
			direction = NORTH;
		} else if((g == KEY_DOWN || g == ltrdwn) && direction != NORTH) {
			direction = SOUTH;
		} else if((g == KEY_LEFT || g == ltrlft) && direction != EAST) {
			direction = WEST;
		} else if((g == KEY_RIGHT || g == ltrrght) && direction != WEST) {
			direction = EAST;
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
				deathclear(0);
				return gameovermenu(mode, border, times, gametime, 0);
			}
		}
		if(level != 0){
			if(head->y == 4 ){
				deathclear(0);
				return gameovermenu(mode, border, times, gametime, 0);
			}
			if(head->x == ACS_VLINE && (head->x != 0 || head->x != maxinx - 1)){
				deathclear(0);
				return gameovermenu(mode, border, times, gametime, 0);
			}
		}
		// Verifica colisao com borda
		if(border == BORDER) {
			if(head->x == maxinx - 1 || head->x == 0 || head->y == maxiny - 1 || head->y == 0) {
				deathclear(0);
				return gameovermenu(mode, border, times, gametime, 0);
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
			mvwprintw(wmain, timery, timerx+10, "0%li", gametime%60);
		}
		if(gametime / 60 <10){
			mvwprintw(wmain, timery, timerx+7, "0");
		}
		wrefresh(wmain);

		// Verifica se acabou o tempo do modo Time attack
		if (mode == MODE_TIMEATK) {
			if(start + times <= time(NULL)){
				deathclear(1);
				return gameovermenu(mode, border, times, gametime, 1);
			}
		}

		// Desenha a cabeca da cobrinha
		mvwaddch(inner, head->y, head->x, ACS_BLOCK);
		wrefresh(inner);

		// Velocidade do jogo
		usleep(200000/gamespeed);
	}
}
