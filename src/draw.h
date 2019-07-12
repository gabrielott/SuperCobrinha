#ifndef DRAW_HEADER
#define DRAW_HEADER

// Constantes para os pares de cores
#define WHITE 0
#define GREEN 1
#define YELLOW 2
#define RED 3
#define CYAN 4

#include <ncurses.h>

#include "snake.h"

// Desenha o titulo do jogo (SUPERCOBRINHA)
void draw_title(int py, int px, int color);

// Desenha uma borda no contorno de certa janela
void draw_border(WINDOW *w);

// Desenha o gamestate no canto inferior direito
void draw_state(void);

// Desenha os creditos
void draw_credits(void);

// Desenha uma snakepart com a cor passada como parametro
void draw_part(Snakepart *part, int color);

#endif