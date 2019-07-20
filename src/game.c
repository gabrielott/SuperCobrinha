#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#include "supercobrinha.h"
#include "datamanagement.h"
#include "menus.h"
#include "snake.h"
#include "food.h"
#include "draw.h"
#include "keyboard.h"

#define INITIAL_SIZE 4
#define FOOD_NUM 1

#define BORDER 0
#define BORDERLESS 1

#define TIMELESS 0
#define TIME_30 30
#define TIME_60 60
#define TIME_180 180
#define TIME_300 300

#define MAXQUE 2

Snakepart *snake[30*14];

time_t start;
time_t gametime;
int score;
int timerx, timery;
int direction;
int grow;
int maxindex;
int cont = 0;
int lastg;
int fila[MAXQUE + 1];
float gamespeed = 1;
int timesq, map, spe;
int times;
coord Prev;

Food *foods[FOOD_NUM];

void showtime(time_t gametime) {
	mvwprintw(wmain, timery, timerx, "Tempo:  %li:%li", gametime/60, gametime%60);
	if(gametime % 60 <10){
		mvwprintw(wmain, timery, timerx+10, "0%li", gametime%60);
	}
	if(gametime / 60 <10){
		mvwprintw(wmain, timery, timerx+7, "0");
	}
	wrefresh(wmain);
}

void set_game(void) {
	// Inicializacao de variaveis
	direction = -1;
	grow = 0;
	score = 0;
	maxindex = INITIAL_SIZE - 1;

	// Inicializacao da queue
	for (int i = 0; i <= MAXQUE; i++) {
		fila[i] = 0;
	}
	cont = 0;

	// coordenadas do timer e do placar
	timerx = ((maxx - 32) / 2) - 14;
	timery = 8;
	
	// Inicializao de todas as comidas
	foods[0] = newfood('o', TRUE, 0);
	//foods[1] = newfood('!', TRUE, 7);

	// Inicializacao do mapa, tempo e velocidade, salvos no arquivo options.dat
	loadoptions(NULL, &timesq, &map, &spe);
	int timevet[] = {TIMELESS, TIME_30, TIME_60, TIME_180, TIME_300};
	times = timevet[timesq];
	float speedvet[] = {0.6, 1, 1.8, 3};
	gamespeed = speedvet[spe];

	// Desenho do estado inicial do campo de jogo
	wclear(inner);
	draw_border(inner);

	for(int i = 0; i < INITIAL_SIZE; i++) {
		snake[i] = newpart(i, maxiny / 2 + i, maxinx / 2);
		draw_part(snake[i], i == 0 ? GAMECORES.corSnakeHead : GAMECORES.corSnakePart);
	}

	showtime(times);
	mvwprintw(wmain, timery+2, timerx,"Score: %d", score);
	wrefresh(inner);
	updatestate(READY);

	// Espera o jogador fazer o movimento inicial
	Prev = Active;
	nodelay(inner, FALSE);
	while(!check_move(Prev, Active)) {
		g = wgetch(inner);
		key_command();
	}
	Active = Prev;
	if(g == ' ' || g == '\n') {
		g = KEY_UP;
	}
	lastg = g;
	g = 0;

	nodelay(inner, TRUE);
}

void clear_death(int deathmode) {
	char *mensagem[] = {"Você perdeu", "O tempo acabou", "Você venceu!", ""};
	updatestate(DEATH);
	mvwprintw(inner, 3, (maxinx - strlenunicode(mensagem[deathmode])) / 2, mensagem[deathmode]);
	wrefresh(inner);
	killsnake(snake, maxindex + 1);
	if(deathmode != 3) while(wgetch(inner) == ERR);
}

int game_start(void) {
	set_game();
	updatestate(RUNNING);

	// Tempo a partir do qual a partida comeca
	start = time(NULL);

	// Loop principal
	for(;;) {
		mvwprintw(wmain, timery+2, timerx,"Score: %d", score);
		wrefresh(wmain);

		// Atualiza o tempo de jogo
		if (times != TIMELESS) {
			gametime = start + times - time(NULL);
		} else {
			gametime = time(NULL) - start;
		}

		// Correcao de bug de multiplos inputs
		// Adiciona caracter pressionado na fila de execucao
		while ((fila[cont] = wgetch(inner)) != ERR) { // O codigo de pause mais elaborado do universo (funciona)
			if(key_pressed(fila[cont], 3, '\n', ' ', 27)) {
				updatestate(PAUSED);
				if(fila[cont] == 27) {
					// Menu de pause ao pressionar esc
					WINDOW *pause = newwin(16, 32, 7, (maxx - 32) / 2);
					keypad(pause, TRUE);
					draw_border(pause);

					char *message = "Menu de pause";
					mvwprintw(pause, 3, (maxinx - strlenunicode(message)) / 2, message);

					char *options[] = {"Voltar ao jogo", "Sair"};
					if(makeselector(pause, 2, options)) { 
						clear_death(3);
						clear_gameover();
						wrefresh(wmain);
						return 1;
					}
					redrawwin(inner);
					wrefresh(inner);
				}
				else {
					// Pauses com enter e barra de espaco
					int leav;
					nodelay(inner, FALSE);
					do {
						leav = wgetch(inner);
					} while(leav != '\n' && leav != ' ');
					nodelay(inner, TRUE);
				}

				// Zera a fila para evitar que a cobra se mova apos o pause
				for (int i = 0; i <= MAXQUE; i++) {
					fila[i] = 0;
				}
				cont = 0;
				// Ajusta o timer
				if (times != TIMELESS) {
					start = time(NULL) - (times - gametime);
				} else {
					start = time(NULL) - gametime;
				}
				updatestate(RUNNING);
			}
			// Avanca o indice da fila
			else if (cont < MAXQUE) {
				cont++;
			}
		}

		// Executa input conforme a ordem da fila de execucao
		if (fila[0] != 0 && fila[0] != ERR) {
			// Captura o primeiro da fila para execucao
			g = fila[0];
			// Anda com a fila
			for (int i = 0; i < MAXQUE; i++){
				fila[i] = fila[i+1];
			}
			// Esvazia o final da fila
			fila[MAXQUE] = 0;
			if (cont > 0){	
				cont--;
			}
		}

		// Atualiza a direcao da cobrinha
		if(g == 0 || g == ERR) {
			g = lastg;
		}
		Prev = Active;
		key_command();
		lastg = g;

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
		tail->x = Active.x + 1;
		tail->y = Active.y + 1;

		for(int i = 0; i < maxindex + 1; i++) {
			snake[i]->index++;
		}

		Snakepart *oldhead = head;
		tail->index = 0;
		head = tail;

		// Verifica colisao com cada tipo de comida
		for(int i = 0; i < FOOD_NUM; i++) {
			if(checkfoodcolision(foods[i], head)) {
				grow = 1;
				score++;
			}
		}

		// Verifica se o jogador venceu o jogo (na teoria)
		if(maxindex == 30*14-1) {
			clear_death(2);
			return menu_gameover(map, timesq, gametime, 2);
		}

		// Verifica colisao com borda
		if(map == BORDER) {
			if(Prev.y == Active.y && Prev.x != Active.x - 1 && Prev.x != Active.x + 1) {
				clear_death(0);
				return menu_gameover(map, timesq, gametime, 0);
			}
			if(Prev.x == Active.x && Prev.y != Active.y - 1 && Prev.y != Active.y + 1) {
				clear_death(0);
				return menu_gameover(map, timesq, gametime, 0);
			}
		}
		
		// Verifica colisao com a propria cobrinha
		for(int i = 0; i < maxindex + 1; i++) {
			if(snake[i] == head) continue;
			if(snake[i]->x == head->x && snake[i]->y == head->y) {
				clear_death(0);
				return menu_gameover(map, timesq, gametime, 0);
			}
		}

		// Exibe o tempo de jogo
		showtime(gametime);

		// Verifica se acabou o tempo do modo Time attack
		if (times != TIMELESS) {
			if(start + times <= time(NULL)) {
				clear_death(1);
				return menu_gameover(map, timesq, gametime, 1);
			}
		}

		// Desenha a cabeca da cobrinha
		draw_part(oldhead, GAMECORES.corSnakePart);
		draw_part(head, GAMECORES.corSnakeHead);

		wrefresh(inner);

		// Velocidade do jogo
		usleep(200000.0/gamespeed);
	}
}
