#include <ncurses.h>
#include <stdio.h>
#include <stdarg.h>

#include "supercobrinha.h"
#include "datamanagement.h"
#include "draw.h"

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

// Funcao para verificar se uma ou mais teclas foram pressionadas
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

// Opcao simples para sair do gamestate
void key_sair() {
	leave = 1;
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

// Verifica algumas condicoes especiais no menu de opcoes
void check_special() {
	if(Active.y == 0) {
		setletters(Active.x);
	} else if(Active.y == 4) {
		GAMECORES = setscheme(Active.x);
		redraw_all();
	}
}

void key_esq_2() {
	key_esq();
	check_special();
}

void key_dir_2() {
	key_dir();
	check_special();
}

// Funcoes para os pauses do jogo, nao implementadas
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
	if(GAMESTATE == IDLE || GAMESTATE == PAUSED || GAMESTATE == DEATH || GAMESTATE == CREDITOS) { // 0, 3, 4 e 11 a principio desnecessarios
		PRESS.UP = &key_nulo;
		PRESS.DOWN = &key_nulo;
		PRESS.LEFT = &key_nulo;
		PRESS.RIGHT = &key_nulo;
		PRESS.SPACE = &key_nulo;
		PRESS.ESC = &key_nulo;
		PRESS.ANY = &key_nulo;
	} else if(GAMESTATE == READY) { //1
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
	} else if(GAMESTATE == RUNNING) { //2
		PRESS.UP = &key_cima;
		PRESS.DOWN = &key_baixo;
		PRESS.LEFT = &key_esq;
		PRESS.RIGHT = &key_dir;
		PRESS.SPACE = &key_pause;
		PRESS.ESC = &key_pausemenu;
		PRESS.ANY = &key_nulo;
	} else if(GAMESTATE == MPRINCIPAL || GAMESTATE == MGAMEOVER || GAMESTATE == MPAUSE) { //5, 8 e 10 Sao os menus que usam a makeselector
		PRESS.UP = &key_cima;
		PRESS.DOWN = &key_baixo;
		PRESS.LEFT = &key_nulo;
		PRESS.RIGHT = &key_nulo;
		PRESS.SPACE = &key_select;
		PRESS.ESC = &key_nulo;
		PRESS.ANY = &key_nulo;
	} else if(GAMESTATE == MOPTIONS) { //6
		PRESS.UP = &key_cima;
		PRESS.DOWN = &key_baixo;
		PRESS.LEFT = &key_esq_2;
		PRESS.RIGHT = &key_dir_2;
		PRESS.SPACE = &key_sair;
		PRESS.ESC = &key_sair;
		PRESS.ANY = &key_nulo;
	} else if(GAMESTATE == MSCOREBOARD) { //7
		Active.x = 0; // mapa
		Active.mod_n = 2;
		Active.y = 0; // timer
		Active.mod_m = 5;
		PRESS.UP = &key_cima;
		PRESS.DOWN = &key_baixo;
		PRESS.LEFT = &key_esq;
		PRESS.RIGHT = &key_dir;
		PRESS.SPACE = &key_sair;
		PRESS.ESC = &key_sair;
		PRESS.ANY = &key_nulo;
	} else if(GAMESTATE == MSAVESCORE) { //9 - NAO IMPLEMENTADO
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
