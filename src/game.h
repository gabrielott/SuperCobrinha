#ifndef GAME_HEADER
#define GAME_HEADER

// Constantes para os diferentes mapas e tempos de jogo
#define BORDER 0
#define BORDERLESS 1

#define TIMELESS 0
#define TIME_30 1
#define TIME_60 2
#define TIME_180 3
#define TIME_300 4

#include "snake.h"

// Index do ultimo pedaco da cobrinha
extern int maxindex;

//Pontuacao do jogo
extern int score;

// Coordenadas do timer
extern int timerx, timery;

// Array de ponteiros de cada parte da cobra
extern Snakepart *snake[30*14];

// Desenha o mapa inicial do jogo e inicializa as variaveis necessarias
void initialsetup(void);

// Inicia o jogo. 
int startgame(void);

#endif
