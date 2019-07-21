#include <ncurses.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>

#include "supercobrinha.h"
#include "game.h"
#include "datamanagement.h"
#include "draw.h"
#include "keyboard.h"

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

	Active.y = 0;
	Active.mod_m = optamt;

	for(;;) {
		for(int i = 0; i < optamt; i++) { 
			if(i == Active.y) {
				mvwchgat(w, (y - optamt * 2) / 2 + i * 2, (x - strlenunicode(options[i])) / 2, strlenunicode(options[i]), A_STANDOUT, GAMECORES.corMenuHL, NULL);
			} else {
				mvwchgat(w, (y - optamt * 2) / 2 + i * 2, (x - strlenunicode(options[i])) / 2, strlenunicode(options[i]), A_NORMAL, GAMECORES.corMenu, NULL);
			}
		}

		wrefresh(w);

		g = wgetch(w);
		key_command();
		if(selecionado == 1) {
			selecionado = 0;
			return Active.y;
		}
	}
}

void menu_savescore(int map, int times, time_t totaltime) {
	wclear(inner);
	draw_border(inner);

	updatestate(MSAVESCORE);

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

void menu_scoreboard(void) {
	wclear(inner);
	draw_border(inner);

	updatestate(MSCOREBOARD);

	char *title = "Hi-scores";
	mvwprintw(inner, 1, (maxinx - strlenunicode(title)) / 2, title);

	nodelay(inner, FALSE);
	int map = BORDER;
	int gtime = 0;
	char *s_times[] = {"Normal", "00:30 ", "01:00 ", "03:00 ", "05:00 "};
	for(int exit = 0; exit != 1;) {
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
		exit = menu_command_2(g, &map, &gtime);

		for(int i = 0; i < size; i++) {
			free(scores[i]);
		}
	}
}

int menu_gameover(int map, int times, time_t totaltime, int deathcase) {
	updatestate(MGAMEOVER);
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
					menu_savescore(map, times, totaltime);
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

void menu_options(void) {
	wclear(inner);
	draw_border(inner);

	updatestate(MOPTIONS);

	char *opt_options[] = {"Layout:", "Timer:", "Mapa:", "Speed:", "Colors:", "Voltar"};
	int opt_amt = 6;

	char *layout_options[] = {"QWERTY", "Colemak"};
	char *time_options[] = {"Normal", "00:30", "01:00", "03:00", "05:00"};
	char *map_options[] = {"Com Borda", "Sem Borda"};
	char *speed_options[] = {"Slow", "Normal", "Fast", "INSANE"};
	char *color_options[] = {"Classic", "Scarlet", "Random"};

	char **opt_index[] = {layout_options, time_options, map_options, speed_options, color_options, opt_options};
	int amt_index[] = {2, 5, 2, 4, 3};

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
	int selected = 0, selX, HLsize, go_back = 0;
	int current[] = {op_teclado, op_tempo, op_mapa, op_speed, GAMECORES.ID};

	void opt_print(void) {
		int strline = 3;
		mvwprintw(inner, strline, 16, layout_options[current[0]]);
		mvwprintw(inner, strline + 2, 16, time_options[current[1]]);
		mvwprintw(inner, strline + 4, 16, map_options[current[2]]);
		mvwprintw(inner, strline + 6, 16, speed_options[current[3]]);
		mvwprintw(inner, strline + 8, 16, color_options[current[4]]);
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
		selected = menu_command_3(g, selected, current, opt_amt, amt_index, &go_back);
		if(go_back == 1) {
			return;
		}

		// Refaz as opcoes para corrigir o highlight
		opt_print();
	}
}

int menu_principal(void) {
	wclear(inner);
	draw_border(inner);

	updatestate(MPRINCIPAL);

	char *options[] = {"Iniciar Jogo", "Scoreboard", "Opções", "Créditos", "Sair"};

	int ans = makeselector(inner, 5, options);

	switch(ans) {
		case 0:
			while(!game_start());
			return 0;
		case 1:
			menu_scoreboard();
			return 0;
		case 2:
			menu_options();
			return 0;
		case 3:
			draw_credits();
			return 0;
		case 4:
			return 1;
	}
	return 0;
}
