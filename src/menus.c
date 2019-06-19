#include <ncurses.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "supercobrinha.h"
#include "game.h"
#include "datamanagement.h"
int level = 0;

void makeborder(WINDOW *w) {
	int x, y;
	getmaxyx(w, y, x);

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
}

int makeselector(WINDOW *w, int optamt, char *options[]) {
	int y, x;
	getmaxyx(w, y, x);

	wrefresh(w);

	for(int i = 0; i < optamt; i++) {
		mvwprintw(w, (y - optamt * 2) / 2 + i * 2, (x - strlen(options[i])) / 2, options[i]);
	}

	int selected = 0;

	for(;;) {
		for(int i = 0; i < optamt; i++) { 
			if(selected == i) {
				mvwchgat(w, (y - optamt * 2) / 2 + i * 2, (x - strlen(options[i])) / 2, strlen(options[i]), A_STANDOUT, COLOR_RED, NULL);
			} else {
				mvwchgat(w, (y - optamt * 2) / 2 + i * 2, (x - strlen(options[i])) / 2, strlen(options[i]), A_NORMAL, COLOR_RED, NULL);
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

int timeatkmenu(int border) {
	wclear(inner);
	makeborder(inner);

	char *options[] = {"00:30", "01:00", "03:00", "05:00", "Voltar"};

	int time;
	switch(makeselector(inner, 5, options)) {
		case 0:
			time = 30;
			break;
		case 1:
			time = 60;
			break;
		case 2:
			time = 180;
			break;
		case 3:
			time = 300;
			break;
		case 4:
			time = 0;
			return 1;
	}
	while(!startgame(MODE_TIMEATK, border, time, 0));
	return 0;
}

int bordermenu(void) {
	wclear(inner);
	makeborder(inner);

	char *options[] = {"Borda", "Sem Borda", "Voltar"};

	switch(makeselector(inner, 3, options)) {
		case 0:
			return BORDER;
		case 1:
			return BORDERLESS;
		case 2:
			return 0;
		default:
			return 0;
	}
}

void credits(void) {
	nodelay(inner, TRUE);

	char *nomes[] = {"SUPERCOBRINHA", "A game designed by:", "Filipe Castelo", "Gabriel Ottoboni", "João Pedro Silva", "Rodrigo Delpreti", "Thank you for playing!"};
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
				mvwprintw(inner, 1+((i+1+startl[j])%14), (maxinx - strlen(nomes[j])) / 2, nomes[j]);
			} 
			if (j == 6 && i < (setup - startl[j])) {
				if (i <= setup - 33) {
					mvwprintw(inner, 1+((setup-6)%14), (maxinx - strlen(nomes[j])) / 2, nomes[j]);
				} else {
					mvwprintw(inner, 1+((i+1+startl[j])%14), (maxinx - strlen(nomes[j])) / 2, nomes[j]);
				}
			}
		}
		wrefresh(inner);

		usleep(450000);
	}
	return;
}

void savescoremenu(int mode, int border, int times, time_t totaltime) {
	wclear(inner);
	makeborder(inner);

	char *mensagem = "Digite suas iniciais";
	mvwprintw(inner, 3, (maxinx - strlen(mensagem)) / 2, mensagem);
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
	s.mode = mode;
	s.border = border;
	s.times = times;
	s.totaltime = totaltime;
	savescore(&s);
}

void scoreboardmenu(void) {
	int distanceuntiltab(char *tabs[], int tab) {
		int distance = 0;
		for(int i = 0; i < tab - 1; i++) {
			distance += strlen(tabs[i]) + 1;
		}
		return distance;
	}

	wclear(inner);
	makeborder(inner);

	char *title = "Hi-scores";
	mvwprintw(inner, 1, (maxinx - strlen(title)) / 2, title);

	const int tabamnt = 2;
	char *tabs[] = {"Clássico", "Time Attack"};
	int totallen = tabamnt - 1;

	for(int i = 0; i < tabamnt; i++) {
		totallen += strlen(tabs[i]);
	}

	for(int i = 0; i < tabamnt; i++) {
		mvwprintw(inner, 3, (maxinx - totallen) / 2 + distanceuntiltab(tabs, i + 1), i == tabamnt - 1 ? "%s" : "%s|", tabs[i]);
	}


	nodelay(inner, FALSE);
	int selected = 0;
	int border = BORDER;
	int gtime = 0;
	int times[] = {30, 60, 180, 300};
	for(;;) {
		for(int i = 0; i < tabamnt; i++) { 
			if(selected == i) {
				mvwchgat(inner, 3, (maxinx - totallen) / 2 + distanceuntiltab(tabs, i + 1), strlen(tabs[i]), A_STANDOUT, COLOR_RED, NULL);
			} else {
				mvwchgat(inner, 3, (maxinx - totallen) / 2 + distanceuntiltab(tabs, i + 1), strlen(tabs[i]), A_NORMAL, COLOR_RED, NULL);
			}
		}

		Score *scores[10];
		int size;
		switch(selected) {
			case 0:
				size = loadscores(scores, MODE_CLASSIC, border, times[gtime]);
				mvwprintw(inner, 1, maxinx - 5, "    ");
				break;
			case 1:
				size = loadscores(scores, MODE_TIMEATK, border, times[gtime]);
				mvwprintw(inner, 1, maxinx - 5, "%03ds", times[gtime]);
				break;
		}

		mvwprintw(inner, 1, 1, "%s", border == BORDER ? "Borda" : "     ");

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
			border = border == BORDER ? BORDERLESS : BORDER;
		} else if(g == KEY_DOWN || g == ltrdwn) {
			gtime = gtime == 3 ? 0 : gtime + 1;
		} else if(g == ' ' || g == '\n') {
			return;
		}

	}
}

void gameoverclear(void) {
	mvwprintw(wmain,timery,timerx,"            ");
	mvwprintw(wmain,timery+2,timerx,"          ");
	wrefresh(wmain);
}

int gameovermenu(int mode, int border, int times, time_t totaltime, int deathcase) {
	int exit = 0;
	int salvo = 0;
	while(!exit) {
		wclear(inner);
		makeborder(inner);

		char *mensagem[] = {"Voce perdeu", "O tempo acabou"};
		mvwprintw(inner, 3, (maxinx - strlen(mensagem[deathcase])) / 2, mensagem[deathcase]);

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
					savescoremenu(mode, border, times, totaltime);
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
int optionslevel(void) {
	wclear(inner);
	makeborder(inner);
	char *options[] = {"1", "2", "3", "4", "5", "Voltar"};
	switch(makeselector(inner, 6, options)) {
		case 0:
			level = 1;
			break;
		case 1:
			level = 2;
			break;
		case 2:
			level = 3;
			break;
		case 3:
			level = 4;
			break;
		case 4:
			level = 5;
			break;
		case 5:
			level = 0;
			return 1;
		default:
			break;
	}
	while(!startgame(MODE_CLASSIC, 2, 0, level));
	return 0;
}

void optionsmenu(void) {
	int exit = 0;
	while(!exit) {
		wclear(inner);
		makeborder(inner);

		char *options[] = {"Usar layout Colemak", "Usar layout QWERTY", "Créditos", "Voltar"};

		exit = 1;
		switch(makeselector(inner, 4, options)) {
			case 0:
				setletters(LTR_COLEMAK);
				saveoptions(LTR_COLEMAK);
				break;
			case 1:
				setletters(LTR_QWERTY);
				saveoptions(LTR_QWERTY);
				break;
			case 2:
				credits();
				exit = 0;
				break;
			case 3:
				break;
		}
	}
}

int mainmenu(void) {
	wclear(inner);
	makeborder(inner);
	char *options[] = {"Clássico", "Time Attack", "Niveis", "Scoreboard", "Opções", "Sair"};

	int ans = makeselector(inner, 6, options);
	int border;

	if(ans == 5) return 1;
	if(ans != 4 && ans != 3 && ans != 2) {
		border = bordermenu();
		if(border == 0) return 0;
	}

	switch(ans) {
		case 0:
			while(!startgame(MODE_CLASSIC, border, 0, 0));
			return 0;
		case 1:
			while(timeatkmenu(border)) {
				border = bordermenu();
				if(border == 0) break;
			}
			return 0;
		case 2:
			optionslevel();
			return 0;
		case 3:
			scoreboardmenu();
			return 0;
		case 4:
			optionsmenu();
			return 0;
		case 5:
			return 1;
	}
	return 0;
}
