#ifndef SUPERCOBRINHA_HEADER
#define SUPERCOBRINHA_HEADER

// Constantes para os layouts disponiveis
#define LTR_QWERTY 0
#define LTR_COLEMAK 1

// Constantes para os gamestates
#define IDLE 0
#define READY 1
#define RUNNING 2
#define PAUSED 3
#define DEATH 4
#define MPRINCIPAL 5
#define MOPTIONS 6
#define MSCOREBOARD 7
#define MGAMEOVER 8
#define MSAVESCORE 9
#define MPAUSE 10
#define CREDITOS 11

#include <ncurses.h>

// Coordenadas y e x maximas da janela principal
int maxy, maxx;

// Coordenadas y e x maximas da janela interna
int maxiny, maxinx;

// Coordendas y e x do meio da janela principal
int middlex, middley;

// Teclas utilizadas para movimento (variam de acordo com layout selecionado)
int ltrup, ltrdwn, ltrrght, ltrlft;

// Variavel que indica o estado do jogo
int GAMESTATE;

// Layout selecionado atualmente
int layout;

// Janelas principal e interna
WINDOW *wmain, *inner;

// Atualiza as variaveis maxy, maxx, maxiny, maxinx, middlex, middley com o tamanho
// atual das janelas
void updatesize(void);

// Aceita LTR_COLEMAK ou LTR_QWERTY como argumento e modifica as variaveis ltrup,
// ltrdwn, ltrrght, ltrlft de acordo
void setletters(int l);

// Atualiza o estado atual do jogo
void updatestate(int state);

#endif
