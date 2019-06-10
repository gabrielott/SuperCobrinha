#ifndef FOOD_HEADER
#define FOOD_HEADER

#include "snake.h"

// Definicao do tipo Food, representacao de um item generico que aparece
// com certa raridade periodicamente no campo de jogo.
typedef struct Food {
	// Caracter que sera usado para representar o item no campo de jogo
	char character;

	// 1 se ha algum item representado pelo objeto no mapa, se nao, 0.
	// Essa propriedade nao deve ser modificada manualmente.
	int isonmap;

	// 1 se deve haver no maximo 1 item desse tipo no mapa ao mesmo tempo.
	int isunique;

	// Inteiro de 0 a 10 que representa a chance do item aparecer em um loop
	// do jogo. 0 significa que ele sempre aparece (a nao ser que a propriedade
	// isunique e inonmap sejao 1 ao mesmo tempo) e 10 significa muito raro.
	int rarity;
} Food;

// Inicializador de um novo objeto do tipo Food.
Food *newfood(char c, int unique, int rarity);

// Tenta colocar o item f em algum lugar da janela w. A tentativa falhara em alguns casos:
// Items com raridade acima de 0 tem chance de nao serem gerados. O item sera gerado em uma
// coordenada aleatoria da janela, excluindo as bordas e coordenadas ocupadas pela cobrinha.
void generatefood(WINDOW *w, Food *f);

// Verifica se o item colidiu com uma parte da cobra. Retorn 1 se sim e 0 se nao.
int checkfoodcolision(Food *f, Snakepart *s);

#endif
