#include <ncurses.h>
#include <string.h>

#include "supercobrinha.h"
#include "game.h"
#include "datamanagement.h"

void makeborder(WINDOW *w) { //desenha as bordas nas janelas
	int x, y;
	getmaxyx(w, y, x);

	mvwprintw(w, 0, 0, "+"); //os cantos
	mvwprintw(w, 0, x - 1, "+");
	mvwprintw(w, y - 1, 0, "+");
	mvwprintw(w, y - 1, x - 1, "+");

	for(int i = 1; i < x - 1; i++) { //borda superior e inferior
		mvwprintw(w, 0, i, "=");
		mvwprintw(w, y - 1, i, "=");
	}

	for(int i = 1; i < y - 1; i++) { //borda esquerda e direita
		mvwprintw(w, i, 0, "|");
		mvwprintw(w, i, x - 1, "|");
	}
}

int makeselector(WINDOW *w, int optamt, char *options[]) { //funcao que permite selecionar opcoes dadas em um menu
	int y, x;
	getmaxyx(w, y, x);

	wrefresh(w);

	for(int i = 0; i < optamt; i++) { //imprime as opcoes do menu
		mvwprintw(w, (y - optamt * 2) / 2 + i * 2, (x - strlen(options[i])) / 2, options[i]);
	}

	int selected = 0; //inicializa o menu com a primeira opcao selecionada

	for(;;) { //mantem usuario no menu ate que alguma opcao seja selecionada
		for(int i = 0; i < optamt; i++) {  //destaca opcao selecionada, ainda sem usar cores
			if(selected == i) {
				mvwchgat(w, (y - optamt * 2) / 2 + i * 2, (x - strlen(options[i])) / 2, strlen(options[i]), A_STANDOUT, COLOR_RED, NULL);
			} else {
				mvwchgat(w, (y - optamt * 2) / 2 + i * 2, (x - strlen(options[i])) / 2, strlen(options[i]), A_NORMAL, COLOR_RED, NULL);
			}
		}

		wrefresh(w);

		int g = wgetch(w);
		if((g == KEY_UP || g == ltrup) && selected > 0) { //desloca seletor para cima
			selected--;
		} else if((g == KEY_UP || g == ltrup) && selected == 0) { //desloca seletor para cima dando a volta no menu
			selected = optamt - 1;
		} else if((g == KEY_DOWN || g == ltrdwn) && selected < optamt - 1) { //desloca seletor para baixo
			selected++;
		} else if((g == KEY_DOWN || g == ltrdwn) && selected == optamt - 1) { //desloca seletor para baixo
			selected = 0;
		} else if(g == ' ' || g == '\n') { //realiza a funcao selecionada quando enter for pressionado
			return selected;
		}
	}
}

int optionsTIMES(void) { //opcoes de tempo para o modo time attack
	wclear(inner);
	makeborder(inner);
	int times;
	char *optionsTIME[] = {"30", "60", "180", "300", "Voltar"};

	switch(makeselector(inner, 5, optionsTIME)) {
		case 0:
			times = 30;
			break;
		case 1:
			times = 60;
			break;
		case 2:
			times = 180;
			break;
		case 3:
			times = 300;
			break;
		case 4:
			times = 0;
			break;
	}
	return times;
}
void options2(void) { //opcoes de borda para modo time attack
	int times;
	wclear(inner);
	makeborder(inner);

	char *options[] = {"Borda", "Sem Borda", "Voltar"};

	switch(makeselector(inner, 3, options)) {
		case 0:
			times = optionsTIMES();
			if(times == 0)
				options2();
			startgame(MODE_TIMEATK, times, 1);
			break;
		case 1:
			times = optionsTIMES();
			if(times == 0)
				options2();
			startgame(MODE_TIMEATK, times, 0);
			break;
		case 2:
			return;
	}
}


void optionsmenu(void) { //opcoes de teclado
	wclear(inner); //limpa janela
	makeborder(inner); //cria borda da janela inner

	char *options[] = {"Usar layout Colemak", "Usar layout QWERTY", "Cancelar"};

	switch(makeselector(inner, 3, options)) {
		case 0:
			setletters(LTR_COLEMAK);
			saveoptions(LTR_COLEMAK);
			break;
		case 1:
			setletters(LTR_QWERTY);
			saveoptions(LTR_QWERTY);
			break;
		case 2:
			return;
	}
}

int mainmenu(void) { //menu principal
	int times;
	char *options[] = {"Sem bordas", "Com bordas","Time Attack", "Opções", "Sair"};
	wclear(inner); //limpa janela
	makeborder(inner); //cria borda da janela inner
	switch(makeselector(inner, 5, options)) {
		case 0:
			startgame(MODE_BORDERLESS, 0, 0);
			return 0;
		case 1:
			startgame(MODE_BORDER, 0, 0);
			return 0;
		case 2:
			options2();
			return 0;
		case 3:
			optionsmenu();
			return 0;
		case 4:
			return 1;
		default:
			return 0;
	}
}
