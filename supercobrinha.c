#include <ncurses.h>
#include <locale.h>
#include <time.h>
#include <stdlib.h>

#include "menus.h"
#include "datamanagement.h"

#define LTR_COLEMAK 1
#define LTR_QWERTY 2

int maxy, maxx;
int maxiny, maxinx;
int middlex, middley;
int ltrup, ltrdwn, ltrrght, ltrlft;
int layout;

WINDOW *wmain, *inner; //declara as 2 janelas do jogo

void updatesize(void) { //funcao para capturar dimensoes importantes da tela
	getmaxyx(wmain, maxy, maxx);
	getmaxyx(inner, maxiny, maxinx);
	middlex = maxx / 2;
	middley = maxy / 2;
}

void setletters(int l) { //define os caracteres recebidos por cada layout de teclado
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
	mvwprintw(wmain, maxy - 2, 1, "Layout: %s", layout == LTR_COLEMAK ? "Colemak" : "QWERTY"); //exibe layout na parte inferior da tela
	wrefresh(wmain);
}

int main(void) {
	setlocale(LC_ALL, "");
	
	initscr(); //inicializa "tela"
	cbreak();
	noecho();
	curs_set(FALSE); //define se vai utilizar cursor do mouse

	wmain = newwin(LINES, COLS, 0, 0); //declara tamanhos das janelas
	inner = newwin(16, 32, 7, (COLS - 32) / 2);

	updatesize(); //verifica o tamanho atual do terminal e guarda variaveis uteis

	if(maxy < 28 || maxx < 85) { //verifica se o tamanho do terminal eh suficiente para rodar o jogo
		mvwprintw(wmain, middley - 2, (maxx - 25) / 2, "Favor ampliar o terminal,");
		mvwprintw(wmain, middley - 1, (maxx - 25) / 2, "pressione algo para sair.");
		while(!wgetch(wmain));
		endwin();
		return 0;
	}

	keypad(inner, TRUE); //libera uso de setas na janela inner
	setupsaves();
	srand(time(NULL)); //semente para gerar numeros aleatorios

	int py = 1; //coordenada y do titulo
	const int px = (maxx - 72) / 2; //coordenada x do titulo

	makeborder(wmain); //imprime bordas e titulo na janela maior
	mvwprintw(wmain, py++, px, "   _____                                  __         _       __         ");
	mvwprintw(wmain, py++, px, "  / ___/__  ______  ___  ______________  / /_  _____(_)___  / /_  ____ _");
	mvwprintw(wmain, py++, px, "  \\__ \\/ / / / __ \\/ _ \\/ ___/ ___/ __ \\/ __ \\/ ___/ / __ \\/ __ \\/ __ `/");
	mvwprintw(wmain, py++, px, " ___/ / /_/ / /_/ /  __/ /  / /__/ /_/ / /_/ / /  / / / / / / / / /_/ / ");
	mvwprintw(wmain, py++, px, "/____/\\__,_/ .___/\\___/_/   \\___/\\____/_.___/_/  /_/_/ /_/_/ /_/\\__,_/  ");
	mvwprintw(wmain, py, px, "          /_/                                                           ");
	wrefresh(wmain);

	if(loadoptions() == LTR_COLEMAK) { //carrega as opcoes de teclado
		setletters(LTR_COLEMAK);
	} else {
		setletters(LTR_QWERTY);
	}

	int exit = 0;
	while(!exit) {
		exit = mainmenu(); //chama o menu principal
	}

	endwin(); //encerra as janelas
}
