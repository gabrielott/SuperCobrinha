#ifndef GAME_HEADER
#define GAME_HEADER

#define MODE_BORDER 1
#define MODE_BORDERLESS 2
#define MODE_TIMEATK 3

#include "snake.h"

int maxindex;
Snakepart *snake[100];

void initialsetup(void);

void startgame(int mode, int times, int brd);

#endif
