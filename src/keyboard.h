#ifndef KEYBOARD_HEADER
#define KEYBOARD_HEADER

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

// Abaixo estao declaradas algumas variaveis globais para as funcoes do teclado

// Essa guarda os ponteiros para as funcoes de cada tecla. Talvez seja interessante trocar a struct por um vetor.
comandos PRESS;

// Essa recebe wgetch() em multiplos lugares e eh avaliada pela key_command() logo em seguida
int g;

// Essa guarda as coordenadas do ponto na tela que esta atualmente ativo
coord Active;

// Essa indica se o usuario deve sair de algum menu
int leave;

// Essa indica se alguma opcao foi selecionada em um menu ou nao
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