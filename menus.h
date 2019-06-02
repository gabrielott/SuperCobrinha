#ifndef MENUS_HEADER
#define MENUS_HEADER

int centerx(WINDOW *, char *c);

void makeborder(WINDOW *w);

int makeselector(WINDOW *w, int y, int optamt, char *options[]);

void optionsmenu(void);

int mainmenu(void);

#endif
