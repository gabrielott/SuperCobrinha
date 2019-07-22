#ifndef MAP_HEADER
#define MAP_HEADER

typedef struct Barrier {
	int x;
	int y;
	int type;
} Barrier;

typedef struct Map {
	char name[16];
	Barrier barriers[100];
	int barriernum;
} Map;

// O tipo Map define um mapa composto por barreiras que
// pode ser colocado no campo de jogo. O mapa deve ser
// carregado por meio da funcao newmap(), que tem como
// unico argumento o nome do arquivo de texto dentro
// da pasta maps que contem a descricao do mapa. Esses
// arquivos devem ter o seguinte formato:
//
// 	- Primeira linha deve conter o nome do mapa
// 	(entre 1 e 15 caracteres, excluindo o \n)
// 	- As proximas 14 linhas devem conter um desenho
// 	do mapa usando os seguintes caracteres:
// 		- *: Espaco vazio
// 		- h: Linha horizontal
// 		- v: Linha vertical
// 		- q: Canto superior esquerdo
// 		- p: Canto superior direito
// 		- z: Canto inferior esquerdo
// 		- m: Canto inferior direito
// 	- Cada uma dessas 14 linhas deve ter exatamente
// 	30 caracteres (excluindo o \n no final da linha)
//
// 	OBS: Caso qualquer uma dessas regras seja quebrada,
// 	a funcao newmap() retornara NULL. A funcao tambem
// 	retornara NULL caso o arquivo nao possa ser aberto
// 	ou caso nao consiga alocar a memoria necessaria para
// 	carregar o mapa.
Map *newmap(char *filename);

//Desenha o mapa na janela inner e atualiza a janela.
void drawmap(Map *m);

// Retorna 1 caso a Snakepart tenha colidido com alguma
// barreira do mapa e 0 caso contrario.
int checkbarriercollision(Map *m, Snakepart *s);

#endif
