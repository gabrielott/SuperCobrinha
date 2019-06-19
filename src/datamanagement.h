#ifndef DATAMANAGEMENT_HEADER
#define DATAMANAGEMENT_HEADER

// Definicao do tipo Score, representacao de um recorde feito por um jogador.
typedef struct Score {
	// Nome de quem fez o score
	char name[4];

	// Numero de pontos do score
	int points;

	// Modo do jogo ao qual o score referencia
	int mode;

	// Borda do jogo ao qual o score referencia
	int border;
	
	// Tempo selectionado para o time attack
	int times;

	// Tempo total de jogo
	time_t totaltime;
} Score;

// Verifica a existencia dos arquivos de salvamento. Caso algum deles nao exista,
// ele sera criado e inicializado com um valor padrao.
void setupsaves(void);

// Salva a opcao de layout no arquivo options.dat. O unico argumento eh um inteiro
// com o valor LTR_COLEMAK or LTR_QWERTY, a opcao selecionada.
void saveoptions(int o);

// Le a opcao de layout salva e retorna LTR_COLEMAK or LTR_QWERTY.
int loadoptions(void);

// Recebe um argumento do tipo Score e adiciona ele ao arquivo scoreboard.dat.
void savescore(Score *s);

// Le o arquivo scoreboard.dat, e escreve ponteiros para os 10 melhores scores em ordem
// decrescente de certo modo, tipo de borda e tempo no ponteiro passado como primeiro
// argumento. Caso nao hajam 10 scores para serem carregados, a funcao carregara o maior
// numero disponivel, esse numero eh o valor retornado pela funcao.
int loadscores(Score **ptr, int mode, int border, int totaltime);

#endif
