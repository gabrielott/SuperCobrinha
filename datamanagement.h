#ifndef DATAMANAGEMENT_HEADER
#define DATAMANAGEMENT_HEADER

typedef struct Score {
	char *name;
	int points;
} Score;

void setupsaves(void);

void saveoptions(int o);

int loadoptions(void);

void savescore(Score *s);

Score **loadscores(void);

#endif
