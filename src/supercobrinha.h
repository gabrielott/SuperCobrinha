#ifndef SUPERCOBRINHA_HEADER
#define SUPERCOBRINHA_HEADER

// Constantes para os layouts disponiveis
#define LTR_COLEMAK 1
#define LTR_QWERTY 2

#include <ncurses.h>

// Coordenadas y e x maximas da janela principal
int maxy, maxx;

// Coordenadas y e x maximas da janela interna
int maxiny, maxinx;

// Coordendas y e x do meio da janela principal
int middlex, middley;

// Teclas utilizadas para movimento (variam de acordo com layout selecionado)
int ltrup, ltrdwn, ltrrght, ltrlft;

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

#endif