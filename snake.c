#include <stdlib.h>

typedef struct Snakepart {
	int y, x;
	int index;
	int color;
} Snakepart;

Snakepart *getpartwithindex(Snakepart *s[], int t, int index) { //dado um indice, retorna a parte da cobrinha
	for(int i = 0; i < t; i++) {
		if(s[i]->index == index) {
			return s[i];
		}
	}
	return NULL;
}

Snakepart *newpart(int index, int y, int x) { //cria um novo elemento da cobrinha
	Snakepart p;
	p.y = y;
	p.x = x;
	p.index = index;

	Snakepart *ptr = calloc(1, sizeof(Snakepart));
	*ptr = p;

	return ptr;
}

void killsnake(Snakepart *s[], int t) { //limpa a memoria utilizada para a cobrinha
	for(int i = 0; i < t; i++) {
		free(s[i]);
	}
}
