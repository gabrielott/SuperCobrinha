#ifndef FOOD_HEADER
#define FOOD_HEADER

#include "snake.h"
#include "keyboard.h"

// Definicao do tipo Food, representacao de um item generico que aparece
// com certa raridade periodicamente no campo de jogo.
typedef struct Food {
	// Caracter que sera usado para representar o item no campo de jogo
	char character;

	//Coordenadas da comida
	coord onde;

	// 1 se ha algum item representado pelo objeto no mapa, se nao, 0.
	// Essa propriedade nao deve ser modificada manualmente.
	int onmap;

	// Inteiro de 0 a 10 que representa a chance do item aparecer em um loop
	// do jogo. 0 significa que ele sempre aparece (a nao ser que a propriedade
	// isunique e inonmap sejao 1 ao mesmo tempo) e 10 significa muito raro.
	int rarity;

	// Inteiro que diz o "valor nutritivo" de uma comida.
	int size;
} Food;

// Inicializador de um novo objeto do tipo Food.
Food newfood(char c, int rare, int fat);

// Tenta desenhar a comida se ela nao estiver no mapa.
void draw_food(Food *f);

// Verifica se os itens colidiram com uma parte da cobra. Retorna 1 se sim e 0 se nao.
int checkfoodcolision(int quant, ...) ;

#endif
