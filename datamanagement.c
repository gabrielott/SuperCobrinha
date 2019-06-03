#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "supercobrinha.h"

typedef struct Score {
	char *name;
	int points;
} Score;

void setupsaves(void) {
	if(access("options.dat", F_OK) == -1) {
		FILE *f = fopen("options.dat", "w");
		if(f == NULL) {
			exit(1);
		}

		int ltr = LTR_COLEMAK;
		fwrite(&ltr, sizeof(int), 1, f);
		fclose(f);
	}

	if(access("scoreboard.dat", F_OK) == -1) {
		FILE *f = fopen("scoreboard.dat", "w");
		if(f == NULL) {
			exit(1);
		}

		int scores = 0;
		fwrite(&scores, sizeof(int), 1, f);
		fclose(f);
	}
}

void savescore(Score *s) {
	FILE *f = fopen("scoreboard.dat", "a");
	if(f == NULL) {
		exit(1);
	}

	fwrite(s, sizeof(Score), 1, f);
	fclose(f);
}

Score **loadscores(void) {
	FILE *f = fopen("scoreboard.dat", "r");
	if(f == NULL) {
		return NULL;
	}

	int scoreamnt;
	fread(&scoreamnt, sizeof(int), 1, f);

	Score **scores = malloc(scoreamnt * sizeof(Score *));
	for(int i = 0; i < scoreamnt; i++) {
		Score *s = malloc(sizeof(score));
		fread(score, sizeof(Score), 1, f);
		scores[i] = s;
	}

	fclose(f);
	return scores;
}
