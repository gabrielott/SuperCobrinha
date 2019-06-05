#ifndef SUPERCOBRINHA_HEADER
#define SUPERCOBRINHA_HEADER

#define LTR_COLEMAK 1
#define LTR_QWERTY 2

#include <ncurses.h>

int maxy, maxx;
int maxiny, maxinx;
int middlex, middley;
int ltrup, ltrdwn, ltrrght, ltrlft;
int layout;

WINDOW *wmain, *inner;

void updatesize(void);

void setletters(int l);

#endif
