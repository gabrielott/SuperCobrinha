#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "supercobrinha.h"
#include "menus.h"
#include "snake.h"

typedef struct Barrier {
	int x;
	int y;
	int type;
} Barrier;

typedef struct Map {
	char name[16];
	Barrier barriers[100];
	int barriernum;
} Map;

Map *newmap(char *filename) {
	char path[21] = "maps/";
	strcat(path, filename);

	FILE *f = fopen(path, "r");
	if(f == NULL) return NULL;

	char name[17];
	fgets(name, 17, f);
	if(strchr(name, '\n') == NULL) return NULL;

	Map *m = malloc(sizeof(Map));
	if(m == NULL) return NULL;

	strncpy(m->name, name, 15);

	int current = -1;
	for(int i = 0; i < 14; i++) {
		char line[32];
		fgets(line, 32, f);
		if(line[30] != '\n') {
			free(m);
			return NULL;
		}

		for(int ii = 0; ii < 30; ii++) {
			Barrier b;
			b.y = i + 1;
			b.x = ii + 1;

			switch(line[ii]) {
				case '*':
					continue;
				case 'h':
					b.type = ACS_HLINE;
					break;
				case 'v':
					b.type = ACS_VLINE;
					break;
				case 'q':
					b.type = ACS_ULCORNER;
					break;
				case 'p':
					b.type = ACS_URCORNER;
					break;
				case 'z':
					b.type = ACS_LLCORNER;
					break;
				case 'm':
					b.type = ACS_LRCORNER;
					break;
				default:
					free(m);
					return NULL;
			}

			m->barriers[++current] = b;
		}
	}

	m->barriernum = current;

	return m;
}

void drawmap(Map *m) {
	for(int i = 0; i < m->barriernum; i++) {
		mvwaddch(inner, m->barriers[i].y, m->barriers[i].x, m->barriers[i].type);
	}
}

int checkbarriercollision(Map *m, Snakepart *s) {
	for(int i = 0; i < m->barriernum; i++) {
		if(s->y == m->barriers[i].y && s->x == m->barriers[i].x) return 1;
	}
	return 0;
}
