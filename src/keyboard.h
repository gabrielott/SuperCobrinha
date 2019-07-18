#ifndef KEYBOARD_HEADER
#define KEYBOARD_HEADER

// Quatro direcoes padrao que talvez eu tire daqui futuramente
#define NORTH 1
#define SOUTH 2
#define EAST 3
#define WEST 4

// Funcao que verifica se uma variavel retornada pela wgetch corresponde a uma quantidade variavel de inputs validos que fazem a mesma coisa
int key_pressed(int keyvar, int quant, ...);

// Funcao que lida com os inputs do teclado
int key_command(int g, int direction);

int menu_command_1(int g, int selec, int optamt, int *choose);

int menu_command_2(int g, int *map, int *gtime);

int menu_command_3(int g, int selected, int current[], int opt_amt, int amt_index[], int *goback);

#endif