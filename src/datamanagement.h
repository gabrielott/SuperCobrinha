#ifndef DATAMANAGEMENT_HEADER
#define DATAMANAGEMENT_HEADER

// Definicao do tipo Score, representacao de um recorde feito por um jogador.
typedef struct Score {
	// Nome de quem fez o score
	char name[4];

	// Numero de pontos do score
	int points;

	// Mapa do jogo ao qual o score referencia
	int map;
	
	// Tempo selecionado
	int times;

	// Tempo total de jogo
	time_t totaltime;
} Score;

// Verifica a existencia dos arquivos de salvamento. Caso algum deles nao exista,
// ele sera criado e inicializado com um valor padrao.
void setupsaves(void);

// Salva a opcao de layout, timer e mapa no arquivo options.dat.
void saveoptions(int lay, int tim, int map, int spe);

// Le a opcao de layout salva no arquivo, LTR_COLEMAK ou LTR_QWERTY, e guarda no ponteiro passado como parametro.
// Le a opcao de timer salva no arquivo e guarda no ponteiro passado como parametro.
// Le a opcao de mapa salva no arquivo e guarda no ponteiro passado como parametro.
void loadoptions(int *lay, int *tim, int *map, int *spe);

// Recebe um argumento do tipo Score e adiciona ele ao arquivo scoreboard.dat.
void savescore(Score *s);

// Le o arquivo scoreboard.dat, e escreve ponteiros para os 10 melhores scores em ordem
// decrescente de certo modo, tipo de borda e tempo no ponteiro passado como primeiro
// argumento. Caso nao hajam 10 scores para serem carregados, a funcao carregara o maior
// numero disponivel, esse numero eh o valor retornado pela funcao.
int loadscores(Score **ptr, int map, int times);

#endif
