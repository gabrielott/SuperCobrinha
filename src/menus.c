#include <ncurses.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>

#include "supercobrinha.h"
#include "game.h"
#include "datamanagement.h"
#include "draw.h"

int strlenunicode(char *s) {
	int size = 0;
	while(*s != '\0') {
		// Verifica se byte comeca com 0b10
		if((*s & 0xc0) != 0x80) size++;
		s++;
	}
	return size;
}

int makeselector(WINDOW *w, int optamt, char *options[]) {
	int y, x;
	getmaxyx(w, y, x);

	wrefresh(w);

	for(int i = 0; i < optamt; i++) {
		mvwprintw(w, (y - optamt * 2) / 2 + i * 2, (x - strlenunicode(options[i])) / 2, options[i]);
	}

	int selected = 0;

	for(;;) {
		for(int i = 0; i < optamt; i++) { 
			if(i == selected) {
				mvwchgat(w, (y - optamt * 2) / 2 + i * 2, (x - strlenunicode(options[i])) / 2, strlenunicode(options[i]), A_STANDOUT, GAMECORES.corMenuHL, NULL);
			} else {
				mvwchgat(w, (y - optamt * 2) / 2 + i * 2, (x - strlenunicode(options[i])) / 2, strlenunicode(options[i]), A_NORMAL, GAMECORES.corMenu, NULL);
			}
		}

		wrefresh(w);

		int g = wgetch(w);
		if((g == KEY_UP || g == ltrup) && selected > 0) {
			selected--;
		} else if((g == KEY_UP || g == ltrup) && selected == 0) {
			selected = optamt - 1;
		} else if((g == KEY_DOWN || g == ltrdwn) && selected < optamt - 1) {
			selected++;
		} else if((g == KEY_DOWN || g == ltrdwn) && selected == optamt - 1) {
			selected = 0;
		} else if(g == ' ' || g == '\n') {
			return selected;
		}
	}
}

void savescoremenu(int map, int times, time_t totaltime) {
	wclear(inner);
	draw_border(inner);

	char *mensagem = "Digite suas iniciais";
	mvwprintw(inner, 3, (maxinx - strlenunicode(mensagem)) / 2, mensagem);
	mvwprintw(inner, maxiny / 2, (maxinx - 5) / 2, "_ _ _");

	nodelay(inner, FALSE);
	curs_set(TRUE);
	wmove(inner, maxiny / 2, (maxinx - 5) / 2);
	wrefresh(inner);

	int charn = 0;
	int done = 0;
	int g = wgetch(inner);
	char letters[4];

	while(g != '\n' || !done) {
		if(g == KEY_BACKSPACE && charn >= 0) {
			mvwaddch(inner, maxiny / 2, (maxinx - 5) / 2 + charn * 2, '_');
			if(charn > 0) charn--;
			wmove(inner, maxiny / 2, (maxinx - 5) / 2 + charn * 2);
			done = 0;

		// Verifica se o caracter eh um numero ou letra
		} else if(charn <= 2 && ((g >= 48 && g <= 57) || (g >= 65 && g <= 90) || (g >= 97 && g <= 122))) {
			if(g >= 97 && g <= 122) g -= 32;
			mvwaddch(inner, maxiny / 2, (maxinx - 5) / 2 + charn * 2, g);
			letters[charn] = g;

			if(charn == 2) done = 1;

			if(charn < 2) charn++;
			wmove(inner, maxiny / 2, (maxinx - 5) / 2 + charn * 2);
		}
		wrefresh(inner);
		g = wgetch(inner);
	}

	curs_set(FALSE);
	letters[3] = '\0';

	Score s;
	strcpy(s.name, letters);
	s.points = score;
	s.map = map;
	s.times = times;
	s.totaltime = totaltime;
	savescore(&s);
}

void scoreboardmenu(void) {
	wclear(inner);
	draw_border(inner);

	char *title = "Hi-scores";
	mvwprintw(inner, 1, (maxinx - strlenunicode(title)) / 2, title);

	nodelay(inner, FALSE);
	int map = BORDER;
	int gtime = 0;
	char *s_times[] = {"Normal", "00:30 ", "01:00 ", "03:00 ", "05:00 "};
	for(;;) {
		Score *scores[10];
		int size = loadscores(scores, map, gtime);
		mvwprintw(inner, 3, maxinx - 10, "%s", s_times[gtime]);

		mvwprintw(inner, 3, 4, "%s", map == BORDER ? "Com bordas" : "Sem bordas");

		for(int i = 0; i < 10; i++) {
			mvwprintw(inner, 5 + i, 2, "                      ");
		}

		for(int i = 0; i < size; i++) {
			mvwprintw(inner, 5 + i, 2, i == 9 ? "%d - %s %d" : "%d  - %s %d", i + 1, scores[i]->name, scores[i]->points);
		}

		wrefresh(inner);

		int g = wgetch(inner);
		if((g == KEY_RIGHT || g == ltrrght) && map < 1) {
			map++;
		} else if((g == KEY_RIGHT || g == ltrrght) && map == 1) {
			map = 0;
		} else if((g == KEY_LEFT || g == ltrlft) && map > 0) {
			map--;
		} else if((g == KEY_LEFT || g == ltrlft) && map == 0) {
			map = 1;
		} else if((g == KEY_UP || g == ltrup) && gtime > 0) {
			gtime--;
		} else if((g == KEY_UP || g == ltrup) && gtime == 0) {
			gtime = 4;
		} else if((g == KEY_DOWN || g == ltrdwn) && gtime < 4) {
			gtime++;
		} else if((g == KEY_DOWN || g == ltrdwn) && gtime == 4) {
			gtime = 0;
		} else if(g == ' ' || g == '\n') {
			return;
		}

		for(int i = 0; i < size; i++) {
			free(scores[i]);
		}
	}
}

int gameovermenu(int map, int times, time_t totaltime, int deathcase) {
	int exit = 0;
	int salvo = 0;
	while(!exit) {
		wclear(inner);
		draw_border(inner);

		char *mensagem[] = {"Voce perdeu", "O tempo acabou", "Voce venceu!"};
		mvwprintw(inner, 3, (maxinx - strlenunicode(mensagem[deathcase])) / 2, mensagem[deathcase]);

		exit = 1;
		if(salvo) {
			char *options[] = {"Tentar novamente", "Voltar ao menu principal"};

			switch(makeselector(inner, 2, options)) {
				case 0:
					clear_gameover();
					return 0;
				case 1:
					clear_gameover();
					return 1;
				default:
					clear_gameover();
					return 1;
			}
		} else {
			char *options[] = {"Salvar score", "Tentar novamente", "Voltar ao menu principal"};

			switch(makeselector(inner, 3, options)) {
				case 0:
					savescoremenu(map, times, totaltime);
					exit = 0;
					salvo = 1;
					break;
				case 1:
					clear_gameover();
					return 0;
				case 2:
					clear_gameover();
					return 1;
				default:
					clear_gameover();
					return 1;
			}
		}
	}
	return 1;
}

void optionsmenu(void) {
	wclear(inner);
	draw_border(inner);

	char *opt_options[] = {"Layout:", "Timer:", "Mapa:", "Speed:", "Colors:", "Voltar"};
	int opt_amt = 6;

	char *layout_options[] = {"QWERTY", "Colemak"};
	char *time_options[] = {"Normal", "00:30", "01:00", "03:00", "05:00"};
	char *map_options[] = {"Com Borda", "Sem Borda"};
	char *speed_options[] = {"Slow", "Normal", "Fast", "INSANE"};
	char *color_options[] = {"Classic", "Scarlet"};

	char **opt_index[] = {layout_options, time_options, map_options, speed_options, color_options, opt_options};
	int amt_index[] = {2, 5, 2, 4, 2};

	for(int i = 0; i < opt_amt; i++) {
		if(i != (opt_amt - 1)) {
			mvwprintw(inner, 3 + 2*i, 8, opt_options[i]);
		} else {
			mvwprintw(inner, 3 + 2*i, (32 - strlenunicode(opt_options[i]))/2, opt_options[i]);
		}
	}

	// Variaveis auxiliares, codigo deve ser otimizado futuramente
	int op_teclado, op_tempo, op_mapa, op_speed;
	loadoptions(&op_teclado, &op_tempo, &op_mapa, &op_speed);
	int selected = 0, selX, HLsize;
	int current[] = {op_teclado, op_tempo, op_mapa, op_speed, GAMECORES.ID};

	void opt_print(void) {
		mvwprintw(inner, 3, 16, layout_options[current[0]]);
		mvwprintw(inner, 5, 16, time_options[current[1]]);
		mvwprintw(inner, 7, 16, map_options[current[2]]);
		mvwprintw(inner, 9, 16, speed_options[current[3]]);
		mvwprintw(inner, 11, 16, color_options[current[4]]);
	}
	opt_print();

	for(;;) {
		if(selected != (opt_amt - 1)) {
			HLsize = strlenunicode(opt_index[selected][current[selected]]);
			selX = 16;
		} else if(selected == (opt_amt - 1)) {
			selX = 1 + (30 - strlenunicode(opt_options[(opt_amt - 1)]))/2;
			HLsize = 6;
		}

		for(int i = 0; i < opt_amt; i++) {
			mvwchgat(inner, 3 + 2*i, 2, 25, A_NORMAL, GAMECORES.corMenu, NULL);
		}
		mvwchgat(inner, 3 + selected*2, selX, HLsize, A_STANDOUT, GAMECORES.corMenuHL, NULL);

		wrefresh(inner);

		int g = wgetch(inner);
		if(selected != opt_amt - 1) {
			mvwprintw(inner, 3 + 2*selected, 16, "          ");
		}
		// Condicao para se mover no menu para cima e para baixo
		if((g == KEY_UP || g == ltrup) && selected > 0) {
			selected--;
		} else if((g == KEY_UP || g == ltrup) && selected == 0) {
			selected = opt_amt - 1;
		} else if((g == KEY_DOWN || g == ltrdwn) && selected < opt_amt - 1) {
			selected++;
		} else if((g == KEY_DOWN || g == ltrdwn) && selected == opt_amt - 1) {
			selected = 0;
		} 
		// Condicao para alterar as opcoes do menu para os lados
		else if((g == KEY_LEFT || g == ltrlft) && selected != opt_amt - 1) {
			if(current[selected] > 0) {
				current[selected]--;
			} else if(current[selected] == 0) {
				current[selected] = amt_index[selected] - 1;
			}
			// Casos especiais
			if(selected == 0) {
				setletters(current[selected]);
			} else if(selected == 4) {
				GAMECORES = setscheme(current[selected]);
				redraw_all();
			}
		} else if((g == KEY_RIGHT || g == ltrrght) && selected != opt_amt - 1) {
			current[selected] = (current[selected] + 1) % amt_index[selected];
			if(selected == 0) {
				setletters(current[selected]);
			} else if(selected == 4) {
				GAMECORES = setscheme(current[selected]);
				redraw_all();
			}
		}
		// Condicao para salvar as opcoes e sair do menu
		else if((g == ' ' || g == '\n') && selected == opt_amt - 1) {
			saveoptions(current[0], current[1], current[2], current[3]);
			savescheme();
			return;
		}

		// Refaz as opcoes caso seja necessario corrigir
		opt_print();
	}
}

int mainmenu(void) {
	wclear(inner);
	draw_border(inner);

	char *options[] = {"Iniciar Jogo", "Scoreboard", "Opções", "Créditos", "Sair"};

	int ans = makeselector(inner, 5, options);

	switch(ans) {
		case 0:
			while(!startgame());
			return 0;
		case 1:
			scoreboardmenu();
			return 0;
		case 2:
			optionsmenu();
			return 0;
		case 3:
			draw_credits();
			return 0;
		case 4:
			return 1;
	}
	return 0;
}
