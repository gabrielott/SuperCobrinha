#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "supercobrinha.h"
#include "game.h"

typedef struct Score {
	char name[4];
	int points;
	int map;
	int times;
	time_t gametime;
} Score;

void err_check(FILE *fp) {
	if (fp == NULL) {
       fprintf(stderr, "error opening file\n");
       exit(1);
	}
}

void setupsaves(void) {
	if(access("options.dat", F_OK) == -1) {
		FILE *f = fopen("options.dat", "wb");
		err_check(f);

		int ltr = LTR_QWERTY;
		int tim = TIMELESS;
		int map = BORDER;
		int spe = 1;
		fwrite(&ltr, sizeof(int), 1, f);
		fwrite(&tim, sizeof(int), 1, f);
		fwrite(&map, sizeof(int), 1, f);
		fwrite(&spe, sizeof(int), 1, f);
		fclose(f);
	}

	if(access("scoreboard.dat", F_OK) == -1) {
		FILE *f = fopen("scoreboard.dat", "wb");
		err_check(f);
		fclose(f);
	}
}

void saveoptions(int lay, int tim, int map, int spe) {
	FILE *f = fopen("options.dat", "wb");
	err_check(f);

	fwrite(&lay, sizeof(int), 1, f);
	fwrite(&tim, sizeof(int), 1, f);
	fwrite(&map, sizeof(int), 1, f);
	fwrite(&spe, sizeof(int), 1, f);
	fclose(f);
}

void sf_read(FILE *f, int *data){ // fazer com lista de parametros variavel (?)
	if(data == NULL) {
		fseek(f, sizeof(int), SEEK_SET);
	} else {
		fread(data, sizeof(int), 1, f);
	}
}

void loadoptions(int *lay, int *tim, int *map, int *spe) {
	FILE *f = fopen("options.dat", "rb");
	err_check(f);

	sf_read(f, lay);
	sf_read(f, tim);
	sf_read(f, map);
	sf_read(f, spe);

	fclose(f);
}

void savescore(Score *s) {
	FILE *f = fopen("scoreboard.dat", "ab");
	err_check(f);

	fwrite(s, sizeof(Score), 1, f);
	fclose(f);
}

int loadscores(Score **ptr, int map, int times) {
	void slidedownfromindex(Score **s, int index, int t) {
		for(int i = t - 2; i >= index; i--) {
			s[i + 1] = s[i];
		}
	}

	FILE *f = fopen("scoreboard.dat", "rb");
	err_check(f);

	fseek(f, 0, SEEK_END);
	long size = ftell(f);
	rewind(f);
	
	int current = 0;
	int trumped = 0;
	int full = 0;
	for(int i = 0; i < size / sizeof(Score); i++) {
		Score *s = malloc(sizeof(Score));
		fread(s, sizeof(Score), 1, f);

		if(s->map != map) continue;
		if(s->times != times) continue;

		if(current >= 9) full = 1;

		trumped = 0;
		for(int ii = 0; ii < current; ii++) {
			if((s->points > ptr[ii]->points) || (s->points == ptr[ii]->points && s->gametime < ptr[ii]->gametime)) {
				slidedownfromindex(ptr, ii, current + 1);
				ptr[ii] = s;
				trumped = 1;

				if(!full) current++;
				break;
			}
		}
		if(trumped || full) continue;

		ptr[current] = s;
		current++;
	}

	fclose(f);
	return full ? 10 : current;
}
