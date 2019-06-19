#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "supercobrinha.h"
#include "game.h"

typedef struct Score {
	char name[4];
	int points;
	int mode;
	int border;
	int times;
	time_t gametime;
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

int loadscores(Score **ptr, int mode, int border, int times) {
	void slidedownfromindex(Score **s, int index, int t) {
		for(int i = t - 2; i >= index; i--) {
			s[i + 1] = s[i];
		}
	}

	FILE *f = fopen("scoreboard.dat", "rb");
	if(f == NULL) {
		exit(1);
	}

	fseek(f, 0, SEEK_END);
	long size = ftell(f);
	rewind(f);
	
	int current = 0;
	int trumped = 0;
	int full = 0;
	for(int i = 0; i < size / sizeof(Score); i++) {
		Score *s = malloc(sizeof(Score));
		fread(s, sizeof(Score), 1, f);

		if(s->mode != mode) continue;
		if(s->border != border) continue;
		if(s->times != times && mode == MODE_TIMEATK) continue;

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
