#ifndef MENUS_HEADER
#define MENUS_HEADER

// Cria uma borda no contorno de certa janela
void makeborder(WINDOW *w);

// Cria um menu seletor no centro da janela w com optamt opcoes. As opcoes
// sao passadas atraves de um array de strings. A opcao selecionada pelo
// usuario sera retornada atraves de um inteiro entre 0 e optamt - 1
int makeselector(WINDOW *w, int optamt, char *options[]);

// Exibe o menu principal
int mainmenu(void);

// Exibe o menu de gameover
int gameovermenu(int mode, int border, int times, time_t totaltime, int deathcase);

// Limpa wmain apos um gameover
void gameoverclear(void);

#endif
