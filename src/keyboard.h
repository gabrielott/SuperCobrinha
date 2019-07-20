#ifndef KEYBOARD_HEADER
#define KEYBOARD_HEADER

// Quatro direcoes padrao que talvez eu tire daqui futuramente
#define NORTH 1
#define SOUTH 2
#define EAST 3
#define WEST 4

typedef struct comandos {
		// Ponteiros para as funcoes correspondentes de todas as teclas necessarias
	// eu devia fazer um vetor unico ao inves dessa struct feia pra gastar menos memoria mas assim fica mais facil de visualizar
	void (*UP)(void);
	void (*DOWN)(void);
	void (*LEFT)(void);
	void (*RIGHT)(void);
	void (*SPACE)(void);
	void (*ESC)(void);
	void (*ANY)(void);
} comandos;

typedef struct coord {
	int y;
	int mod_m;
	int x;
	int mod_n;
} coord;

// Variaveis GLOBAIS para as funcoes do teclado
comandos PRESS;
int g;
coord Active;
int leave;
int selecionado;

// Funcao que verifica se uma variavel retornada pela wgetch corresponde a uma quantidade variavel de inputs validos que fazem a mesma coisa
int key_pressed(int keyvar, int quant, ...);

// Funcao que verifica se houve algum movimento, indicada pela variavel active
int check_move(coord old, coord new);

// Funcao para setar os ponteiros de cada gamestate
void set_keys();

// Funcao que lida com os inputs do teclado
void key_command();

// Isso aqui em baixo vai sumir
int menu_command_1(int g, int selec, int optamt, int *choose);

int menu_command_2(int g, int *map, int *gtime);

int menu_command_3(int g, int selected, int current[], int opt_amt, int amt_index[], int *goback);

#endif