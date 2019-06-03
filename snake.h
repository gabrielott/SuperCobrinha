#ifndef SNAKE_HEADER
#define SNAKE_HEADER

typedef struct Snakepart {
	int y, x;
	int index;
	int color;
} Snakepart;

Snakepart *getpartwithindex(Snakepart *s[], int t, int index);

Snakepart *newpart(int index, int y, int x);

void killsnake(Snakepart *s[], int t);

#endif
