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

// Funcao para verificar se uma ou mais teclas foram pressionadas
// Retorna 1(true) ou 0(false) 
int key_pressed(int keyvar, int quant, ...);

// Funcao que verifica se houve algum movimento, indicada pela variavel active
int check_move(coord old, coord new);

// Funcao para setar os ponteiros de cada gamestate
void set_keys();

// Funcao que lida com os inputs do teclado
void key_command();

#endif