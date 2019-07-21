#include <ncurses.h>
#include <stdio.h>
#include <stdarg.h>

#include "supercobrinha.h"
#include "datamanagement.h"
#include "draw.h"

#define NORTH 1
#define SOUTH 2
#define EAST 3
#define WEST 4

typedef struct comandos {
	// Ponteiros para as funcoes de todas as teclas necessarias
	// eu devia fazer um vetor unico ao inves dessa struct feia pra gastar menos memoria mas assim fica mais facil de visualizar
	void (*UP)(void);
	void (*DOWN)(void);
	void (*LEFT)(void);
	void (*RIGHT)(void);
	void (*SPACE)(void);
	void (*ESC)(void);
	void (*ANY)(void);
} comandos;

typedef struct coord {
	int y;
	int mod_m;
	int x;
	int mod_n;
} coord;

// Variaveis GLOBAIS para as funcoes do teclado
comandos PRESS;
int g; //fila(?)
coord Active;
int leave;
int selecionado = 0;

int xmody(int x, int y) {
	if(x >= 0) {
		return x % y;
	}
	while(x < 0) {
		x += y;
	}
	return x;
}

int key_pressed(int keyvar, int quant, ...) {
	va_list press;
	va_start(press, quant);
	for(int i = 0; i < quant; i++) {
		if(keyvar == va_arg(press, int)) {
			va_end(press);
			return 1;
		}
	}
	va_end(press);
	return 0;
}

// Funcao que verifica se houve algum movimento
int check_move(coord old, coord new) {
	if(old.x == new.x && old.y == new.y) {
		return 0;
	}
	return 1;
}

// Quando a tecla nao faz nada, eh porque ela esta apontando para essa funcao
void key_nulo() {
	return;
}

// Alguns gamestates saem caso qualquer tecla seja pressionada
void key_sair() {
	return;
	//leave = 1;
}

// Funcoes BEM BASICAS para se movimentar conforme a variavel Active
void key_cima() {
	Active.y = xmody(Active.y - 1, Active.mod_m);
}

void key_baixo() {
	Active.y = xmody(Active.y + 1, Active.mod_m);
}

void key_esq() {
	Active.x = xmody(Active.x - 1, Active.mod_n);
}

void key_dir() {
	Active.x = xmody(Active.x + 1, Active.mod_n);
}

// Funcoes para os pauses do jogo
void key_pause() {

}

void key_pausemenu() {

}

// Funcao para selecionar uma opcao em um menu
void key_select() {
	selecionado = 1;
}

// Funcao para setar os ponteiros de funcoes de cada gamestate.
// Agrupar gamestates cujas teclas executem as mesmas funcoes para nao ficar uma funcao gigantesca
// Inicializacao da variavel Active foi tranferida para ca
void set_keys() {
	if(GAMESTATE == IDLE || GAMESTATE == DEATH || GAMESTATE == CREDITOS) { // 0, 4 e 11 desnecessarios
		PRESS.UP = &key_nulo;
		PRESS.DOWN = &key_nulo;
		PRESS.LEFT = &key_nulo;
		PRESS.RIGHT = &key_nulo;
		PRESS.SPACE = &key_nulo;
		PRESS.ESC = &key_nulo;
		PRESS.ANY = &key_sair;
	} else if(GAMESTATE == READY) { //1 pronto
		Active.y = (maxiny / 2) - 1;
		Active.x = (maxinx / 2) - 1;
		Active.mod_m = maxiny - 2;
		Active.mod_n = maxinx - 2;
		PRESS.UP = &key_cima;
		PRESS.DOWN = &key_nulo;
		PRESS.LEFT = &key_esq;
		PRESS.RIGHT = &key_dir;
		PRESS.SPACE = &key_cima;
		PRESS.ESC = &key_nulo;
		PRESS.ANY = &key_nulo;
	} else if(GAMESTATE == RUNNING) { //2 pronto
		PRESS.UP = &key_cima;
		PRESS.DOWN = &key_baixo;
		PRESS.LEFT = &key_esq;
		PRESS.RIGHT = &key_dir;
		PRESS.SPACE = &key_pause;
		PRESS.ESC = &key_pausemenu;
		PRESS.ANY = &key_nulo;
	} else if(GAMESTATE == PAUSED) { //3 desnecessario
		PRESS.UP = &key_nulo;
		PRESS.DOWN = &key_nulo;
		PRESS.LEFT = &key_nulo;
		PRESS.RIGHT = &key_nulo;
		PRESS.SPACE = &key_sair;
		PRESS.ESC = &key_nulo;
		PRESS.ANY = &key_nulo;
	} else if(GAMESTATE == MPRINCIPAL || GAMESTATE == MGAMEOVER || GAMESTATE == MPAUSE) { //5, 8 e 10 Sao os menus que usam a makeselector
		PRESS.UP = &key_cima;
		PRESS.DOWN = &key_baixo;
		PRESS.LEFT = &key_nulo;
		PRESS.RIGHT = &key_nulo;
		PRESS.SPACE = &key_select;
		PRESS.ESC = &key_nulo;
		PRESS.ANY = &key_nulo;
	} else if(GAMESTATE == MOPTIONS) { //6 - Falta os substates
		PRESS.UP = &key_cima;
		PRESS.DOWN = &key_baixo;
		PRESS.LEFT = &key_esq;
		PRESS.RIGHT = &key_dir;
		PRESS.SPACE = &key_nulo;
		PRESS.ESC = &key_nulo;
		PRESS.ANY = &key_nulo;
	} else if(GAMESTATE == MSCOREBOARD) { //7
		PRESS.UP = &key_cima;
		PRESS.DOWN = &key_baixo;
		PRESS.LEFT = &key_esq;
		PRESS.RIGHT = &key_dir;
		PRESS.SPACE = &key_sair;
		PRESS.ESC = &key_nulo;
		PRESS.ANY = &key_nulo;
	} else if(GAMESTATE == MSAVESCORE) { //9 - Estudar ainda como vou implementar isso aqui, LAYOUT DO TECLADO pode ferrar um pouco essa parte
		PRESS.UP = &key_cima;
		PRESS.DOWN = &key_baixo;
		PRESS.LEFT = &key_nulo;
		PRESS.RIGHT = &key_nulo;
		PRESS.SPACE = &key_select;
		PRESS.ESC = &key_nulo;
		PRESS.ANY = &key_nulo;
	}
}

// Funcao que chama a funcao adequada para cada tecla pressionada
void key_command() {
	if(key_pressed(g, 2, KEY_UP, ltrup)) {
		PRESS.UP();
	} else if(key_pressed(g, 2, KEY_DOWN, ltrdwn)) {
		PRESS.DOWN();
	} else if(key_pressed(g, 2, KEY_LEFT, ltrlft)) {
		PRESS.LEFT();
	} else if(key_pressed(g, 2, KEY_RIGHT, ltrrght)) {
		PRESS.RIGHT();
	} else if(key_pressed(g, 2, ' ', '\n')) {
		PRESS.SPACE();
	} else if(key_pressed(g, 1, 27)) {
		PRESS.ESC();
	} else if(g != 0 && g != ERR) {
		PRESS.ANY();
	}
}

int menu_command_1(int g, int selec, int optamt, int *choose) { // Usada na makeselector
	if(g == KEY_UP || g == ltrup) {
		return xmody(selec - 1, optamt);
	} else if(g == KEY_DOWN || g == ltrdwn) {
		return xmody(selec + 1, optamt);
	} else if(g == ' ' || g == '\n') {
		*choose = 1;
	}
	return selec;
}

int menu_command_2(int g, int *map, int *gtime) { // Usada no menu do scoreboard
	if(g == KEY_RIGHT || g == ltrrght) {
		*map = xmody(*map + 1, 2);
	} else if(g == KEY_LEFT || g == ltrlft) {
		*map = xmody(*map - 1, 2);
	} else if(g == KEY_UP || g == ltrup) {
		*gtime = xmody(*gtime - 1, 5);
	} else if(g == KEY_DOWN || g == ltrdwn) {
		*gtime = xmody(*gtime + 1, 5);
	} else if(g == ' ' || g == '\n') {
		return 1;
	}
	return 0;
}

int menu_command_3(int g, int selected, int current[], int opt_amt, int amt_index[], int *goback) { // Usada no menu de opcoes
	if(selected != opt_amt - 1) {
		mvwprintw(inner, 3 + 2*selected, 16, "          ");
	}
	// Condicao para se mover no menu para cima e para baixo
	if(g == KEY_UP || g == ltrup) {
		return xmody(selected - 1, opt_amt);
	} else if(g == KEY_DOWN || g == ltrdwn) {
		return xmody(selected + 1, opt_amt);
	} 
	// Condicao para alterar as opcoes do menu para os lados (configurar como substates)
	else if((g == KEY_LEFT || g == ltrlft) && selected != opt_amt - 1) {
		current[selected] = xmody(current[selected] - 1, amt_index[selected]);
		// Casos especiais
		if(selected == 0) {
			setletters(current[selected]);
		} else if(selected == 4) {
			GAMECORES = setscheme(current[selected]);
			redraw_all();
		}
	} else if((g == KEY_RIGHT || g == ltrrght) && selected != opt_amt - 1) {
		current[selected] = xmody(current[selected] + 1, amt_index[selected]);
		if(selected == 0) {
			setletters(current[selected]);
		} else if(selected == 4) {
			GAMECORES = setscheme(current[selected]);
			redraw_all();
		}
	}
	// Condicao para salvar as opcoes e sair do menu
	else if((g == ' ' || g == '\n') && selected == opt_amt - 1) {
		saveoptions(current[0], current[1], current[2], current[3]);
		savescheme();
		*goback = 1;
	}
	return selected;
}