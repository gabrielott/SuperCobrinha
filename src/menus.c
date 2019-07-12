#include <ncurses.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>

#include "supercobrinha.h"
#include "game.h"
#include "datamanagement.h"

int strlenunicode(char *s) {
	int size = 0;
	while(*s != '\0') {
		// Verifica se byte comeca com 0b10
		if((*s & 0xc0) != 0x80) size++;
		s++;
	}
	return size;
}

void makeborder(WINDOW *w) {
	int x, y;
	getmaxyx(w, y, x);

	wattron(w, COLOR_PAIR(GREEN));
	mvwaddch(w, 0, 0, ACS_ULCORNER);
	mvwaddch(w, 0, x - 1, ACS_URCORNER);
	mvwaddch(w, y - 1, 0, ACS_LLCORNER);
	mvwaddch(w, y - 1, x - 1, ACS_LRCORNER);

	for(int i = 1; i < x - 1; i++) {
		mvwaddch(w, 0, i, ACS_HLINE);
		mvwaddch(w, y - 1, i, ACS_HLINE);
	}

	for(int i = 1; i < y - 1; i++) {
		mvwaddch(w, i, 0, ACS_VLINE);
		mvwaddch(w, i, x - 1, ACS_VLINE);
	}
	wattroff(w, COLOR_PAIR(GREEN));
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
				mvwchgat(w, (y - optamt * 2) / 2 + i * 2, (x - strlenunicode(options[i])) / 2, strlenunicode(options[i]), A_STANDOUT, GREEN, NULL);
			} else {
				mvwchgat(w, (y - optamt * 2) / 2 + i * 2, (x - strlenunicode(options[i])) / 2, strlenunicode(options[i]), A_NORMAL, GREEN, NULL);
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

void credits(void) {
	nodelay(inner, TRUE);

	char *nomes[] = {"SUPERCOBRINHA", "Desenvolvido por:", "Filipe Castelo", "Gabriel Ottoboni", "João Pedro Silva", "Rodrigo Delpreti", "Obrigado por jogar!"};
	int startl[7] = {-7, 1, 5, 8, 11, 14, 25};
	int i, j;
	int setup = 14*5-1;

	for (i = setup; i >= 0; i--) {
		// Prepara a janela para os creditos serem exibidos
		wclear(inner);
		makeborder(inner);
		wrefresh(inner);

		// Sai dos creditos se qualquer tecla for pressionada
		if(wgetch(inner) != ERR) {
			return;
		}

		// Printa os creditos quando for a vez deles
		for (j=0; j<7; j++) {
			if (j != 6 && i < (setup - startl[j]) && i > (setup - 15 - startl[j])) {
				mvwprintw(inner, 1+((i+1+startl[j])%14), (maxinx - strlenunicode(nomes[j])) / 2, nomes[j]);
			} 
			if (j == 6 && i < (setup - startl[j])) {
				if (i <= setup - 33) {
					mvwprintw(inner, 1+((setup-6)%14), (maxinx - strlenunicode(nomes[j])) / 2, nomes[j]);
				} else {
					mvwprintw(inner, 1+((i+1+startl[j])%14), (maxinx - strlenunicode(nomes[j])) / 2, nomes[j]);
				}
			}
		}
		wrefresh(inner);

		usleep(450000);
	}
	return;
}

void savescoremenu(int map, int times, time_t totaltime) {
	wclear(inner);
	makeborder(inner);

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
	int distanceuntiltab(char *tabs[], int tab) {
		int distance = 0;
		for(int i = 0; i < tab - 1; i++) {
			distance += strlenunicode(tabs[i]) + 1;
		}
		return distance;
	}

	wclear(inner);
	makeborder(inner);

	char *title = "Hi-scores";
	mvwprintw(inner, 1, (maxinx - strlenunicode(title)) / 2, title);

	const int tabamnt = 2;
	char *tabs[] = {"Clássico", "Time Attack"};
	int totallen = tabamnt - 1;

	for(int i = 0; i < tabamnt; i++) {
		totallen += strlenunicode(tabs[i]);
	}

	for(int i = 0; i < tabamnt; i++) {
		mvwprintw(inner, 3, (maxinx - totallen) / 2 + distanceuntiltab(tabs, i + 1), i == tabamnt - 1 ? "%s" : "%s|", tabs[i]);
	}


	nodelay(inner, FALSE);
	int selected = 0;
	int map = BORDER;
	int gtime = 0;
	int times[] = {0, 30, 60, 180, 300};
	for(;;) {
		for(int i = 0; i < tabamnt; i++) { 
			if(selected == i) {
				mvwchgat(inner, 3, (maxinx - totallen) / 2 + distanceuntiltab(tabs, i + 1), strlenunicode(tabs[i]), A_STANDOUT, COLOR_RED, NULL);
			} else {
				mvwchgat(inner, 3, (maxinx - totallen) / 2 + distanceuntiltab(tabs, i + 1), strlenunicode(tabs[i]), A_NORMAL, COLOR_RED, NULL);
			}
		}

		Score *scores[10];
		int size;
		switch(selected) {
			case 0:
				size = loadscores(scores, map, times[gtime]);
				mvwprintw(inner, 1, maxinx - 5, "    ");
				break;
			case 1:
				size = loadscores(scores, map, times[gtime]);
				mvwprintw(inner, 1, maxinx - 5, "%03ds", times[gtime]);
				break;
		}

		mvwprintw(inner, 1, 1, "%s", map == BORDER ? "Cborda" : "Sborda");

		for(int i = 0; i < 10; i++) {
			mvwprintw(inner, 5 + i, 2, "                      ");
		}

		for(int i = 0; i < size; i++) {
			mvwprintw(inner, 5 + i, 2, i == 9 ? "%d - %s %d" : "%d  - %s %d", i + 1, scores[i]->name, scores[i]->points);
		}

		wrefresh(inner);

		int g = wgetch(inner);
		if((g == KEY_RIGHT || g == ltrrght) && selected >= 0 && selected < tabamnt - 1) {
			selected++;
		} else if((g == KEY_RIGHT || g == ltrrght) && selected == tabamnt - 1) {
			selected = 0;
		} else if((g == KEY_LEFT || g == ltrlft) && selected <= tabamnt - 1 && selected > 0) {
			selected--;
		} else if((g == KEY_LEFT || g == ltrlft) && selected == 0) {
			selected = tabamnt - 1;
		} else if(g == KEY_UP || g == ltrup) {
			map = map == BORDER ? BORDERLESS : BORDER;
		} else if(g == KEY_DOWN || g == ltrdwn) {
			gtime = gtime == 3 ? 0 : gtime + 1;
		} else if(g == ' ' || g == '\n') {
			return;
		}

		for(int i = 0; i < size; i++) {
			free(scores[i]);
		}
	}
}

void gameoverclear(void) {
	mvwprintw(wmain,timery,timerx,"            ");
	mvwprintw(wmain,timery+2,timerx,"          ");
	wrefresh(wmain);
}

int gameovermenu(int map, int times, time_t totaltime, int deathcase) {
	void gameoverclear(void) {
		mvwprintw(wmain,timery,timerx,"            ");
		mvwprintw(wmain,timery+2,timerx,"          ");
		wrefresh(wmain);
	}

	int exit = 0;
	int salvo = 0;
	while(!exit) {
		wclear(inner);
		makeborder(inner);

		char *mensagem[] = {"Voce perdeu", "O tempo acabou", "Voce venceu!"};
		mvwprintw(inner, 3, (maxinx - strlenunicode(mensagem[deathcase])) / 2, mensagem[deathcase]);

		exit = 1;
		if(salvo) {
			char *options[] = {"Tentar novamente", "Voltar ao menu principal"};

			switch(makeselector(inner, 2, options)) {
				case 0:
					gameoverclear();
					return 0;
				case 1:
					gameoverclear();
					return 1;
				default:
					gameoverclear();
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
					gameoverclear();
					return 0;
				case 2:
					gameoverclear();
					return 1;
				default:
					gameoverclear();
					return 1;
			}
		}
	}
	return 1;
}

void optionsmenu(void) {
	wclear(inner);
	makeborder(inner);

	char *opt_options[] = {"Layout: ", "Timer: ", "Mapa: ", "Voltar"};
	int opt_amt = 4;

	char *layout_options[] = {"QWERTY ", "Colemak"};
	char *time_options[] = {"Normal", "00:30 ", "01:00 ", "03:00 ", "05:00 "};
	char *map_options[] = {"Com Borda", "Sem Borda"};

	for(int i = 0; i < opt_amt; i++) {
		if(i != 3) {
			mvwprintw(inner, 3 + 2*i, 8, opt_options[i]);
		} else {
			mvwprintw(inner, 3 + 2*i, (32 - strlenunicode(opt_options[i]))/2, opt_options[i]);
		}
	}

	int op_teclado, op_tempo, op_mapa; 
	loadoptions(&op_teclado, &op_tempo, &op_mapa);

	mvwprintw(inner, 3, 16, layout_options[op_teclado]);
	mvwprintw(inner, 5, 16, time_options[op_tempo]);
	mvwprintw(inner, 7, 16, map_options[op_mapa]);

	int selected = 0, selX;

	for(;;) {
		if(selected == 3) {
			selX = (32 - strlenunicode(opt_options[4]))/2;
		} else {
			selX = 16;
		}

		for(int i = 0; i < opt_amt; i++) {
			mvwchgat(inner, 3 + 2*i, 2, 30, A_NORMAL, GREEN, NULL);
		}
		mvwchgat(inner, 3 + selected*2, selX, 8, A_STANDOUT, GREEN, NULL);

		wrefresh(inner);

		int g = wgetch(inner);
		// Condicao para se mover no menu
		if((g == KEY_UP || g == ltrup) && selected > 0) {
			selected--;
		} else if((g == KEY_UP || g == ltrup) && selected == 0) {
			selected = opt_amt - 1;
		} else if((g == KEY_DOWN || g == ltrdwn) && selected < opt_amt - 1) {
			selected++;
		} else if((g == KEY_DOWN || g == ltrdwn) && selected == opt_amt - 1) {
			selected = 0;
		} 
		// Condicao para alterar as opcoes do menu
		else if(g == KEY_LEFT || g == ltrlft){
			if(selected == 0 && op_teclado > 0) {
				op_teclado--;
				setletters(op_teclado);
			} else if(selected == 1 && op_tempo > 0) {
				op_tempo--;
			} else if(selected == 2 && op_mapa > 0) {
				op_mapa--;
			}
		} else if(g == KEY_RIGHT || g == ltrrght) {
			if(selected == 0 && op_teclado < 1) {
				op_teclado++;
				setletters(op_teclado);
			} else if(selected == 1 && op_tempo < 4) {
				op_tempo++;
			} else if(selected == 2 && op_mapa < 1) {
				op_mapa++;
			}
		}
		// Condicao para salvar as opcoes e sair do menu
		else if((g == ' ' || g == '\n') && selected == 3) {
			saveoptions(op_teclado, op_tempo, op_mapa);
			return;
		}

		// Refaz as opcoes caso seja necessario corrigir
		mvwprintw(inner, 3, 16, layout_options[op_teclado]);
		mvwprintw(inner, 5, 16, time_options[op_tempo]);
		mvwprintw(inner, 7, 16, map_options[op_mapa]);
	}
}

int mainmenu(void) {
	wclear(inner);
	makeborder(inner);

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
			credits();
			return 0;
		case 4:
			return 1;
	}
	return 0;
}
