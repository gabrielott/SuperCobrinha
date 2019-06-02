#ifndef SUPERCOBRINHA_HEADER
#define SUPERCOBRINHA_HEADER

#define LTR_COLEMAK 1
#define LTR_QWERTY 2

int maxx, maxy;
int middlex, middley;
int ltrup, ltrdwn, ltrrght, ltrlft;
int layout;

void updatesize(void);

void setletters(int l);

#endif
