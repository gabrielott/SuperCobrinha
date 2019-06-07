#ifndef SNAKE_HEADER
#define SNAKE_HEADER

// Define uma coordenada que faz parte da cobrinha
typedef struct Snakepart {
	// Coordenadas da parte
	int y, x;

	// Posicao da parte no contexto da cobrinha
	int index;

	// Cor da parte
	int color;
} Snakepart;

// Retorna um ponteiro para a parte com certo index dentro de um
// array de Snake * com tamanho t
Snakepart *getpartwithindex(Snakepart *s[], int t, int index);

// Inicializa um objeto Snake e retorna seu ponteiro
Snakepart *newpart(int index, int y, int x);

// Desaloca a memoria usada por um array de Snake *
void killsnake(Snakepart *s[], int t);

#endif
