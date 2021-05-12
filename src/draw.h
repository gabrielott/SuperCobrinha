#ifndef DRAW_HEADER
#define DRAW_HEADER

// Constantes para os pares de cores
#define WHITE 0
#define GREEN 1
#define YELLOW 2
#define RED 3
#define CYAN 4

// Constantes para os esquemas de cores
#define CLASSIC 0
#define SCARLET 1

#include <ncurses.h>

#include "snake.h"

// Definicao da struct para os esquemas de cores
typedef struct Scheme {
	// Qual o esquema selecionado
	int ID;

	// Cor das bordas usadas na funcao draw_border
	int corBorder;

	// Cor do titulo
	int corTitle;

	// Cor da cabeca da cobrinha
	int corSnakeHead;

	// Cor padrao da snakepart
	int corSnakePart;

	// Cor padrao das comidas;
	int corFood;

	// Cor das opcoes do menu
	int corMenu;

	// Cor do highlight usado nos menus
	int corMenuHL;

	// Cor dos creditos a serem exibidos
	int corCredits;

	// Cor das mensagens de status exibidas nos cantos inferiores da tela
	int corStatus;
} Scheme;

// Variavel global que guarda as cores atualmente utilizadas no jogo
extern Scheme GAMECORES;

// Retorna o esquema de cores selecionado
Scheme setscheme(int choice);

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

// Redesenha tudo que estiver na tela
void redraw_all(void);

// Limpa a exibicao do timer e do score
void clear_gameover(void);

#endif
