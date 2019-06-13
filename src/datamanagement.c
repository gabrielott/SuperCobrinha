#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "supercobrinha.h"

typedef struct Score {
	char *name;
	int points;
	int mode;
	int border;
	long gametime;
} Score;

void setupsaves(void) {
	if(access("options.dat", F_OK) == -1) {
		FILE *f = fopen("options.dat", "wb");
		if(f == NULL) {
			exit(1);
		}

		int ltr = LTR_COLEMAK;
		fwrite(&ltr, sizeof(int), 1, f);
		fclose(f);
	}

	if(access("scoreboard.dat", F_OK) == -1) {
		FILE *f = fopen("scoreboard.dat", "wb");
		if(f == NULL) {
			exit(1);
		}
	}
}

void saveoptions(int o) {
	FILE *f = fopen("options.dat", "wb");
	if(f == NULL) {
		exit(1);
	}

	fwrite(&o, sizeof(int), 1, f);
	fclose(f);
}

int loadoptions(void) {
	FILE *f = fopen("options.dat", "rb");
	if(f == NULL) {
		exit(1);
	}

	int o;
	fread(&o, sizeof(int), 1, f);

	fclose(f);
	return o;
}

void savescore(Score *s) {
	FILE *f = fopen("scoreboard.dat", "ab");
	if(f == NULL) {
		exit(1);
	}

	fwrite(s, sizeof(Score), 1, f);
	fclose(f);
}

Score **loadscores(int mode, int border) {
	FILE *f = fopen("scoreboard.dat", "r");
	if(f == NULL) {
		return NULL;
	}

	int amount = 0;
	Score **scores = malloc(10 * sizeof(Score *));
//	while(!feof(f)) {
//		Score s;
//		fread(&s, sizeof(Score), 1, f);
//
//		if(s.mode != mode) continue;
//		if(s.border != border) continue;
//
//		for(int i = 0; i < amount; i++) {
//			if(s > scores[i]) {
//
//			}
//		}
//	}

	fclose(f);
	return scores;
}
