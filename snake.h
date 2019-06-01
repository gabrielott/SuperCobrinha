typedef struct Snakepart {
	int y, x;
	int index;
	int color;
} Snakepart;

Snakepart *getpartwithindex(Snakepart *s[], int t, int index);

Snakepart *newpart(int index, int y, int x);
