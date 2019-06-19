#ifndef GAME_HEADER
#define GAME_HEADER

// Constantes para os diferentes modos de jogo
#define BORDER 1
#define BORDERLESS 2

#define MODE_CLASSIC 1
#define MODE_TIMEATK 2

#include "snake.h"

// Index do ultimo pedaco da cobrinha
int maxindex;

//Pontuacao do jogo
int score;

// Coordenadas do timer
int timerx, timery;

// Array de ponteiros de cada parte da cobra
Snakepart *snake[30*14];

// Desenha o mapa inicial do jogo e inicializa as variaveis necessarias
void initialsetup(void);

// Inicia o jogo. 
int startgame(int mode, int border, int times, int level);

#endif
