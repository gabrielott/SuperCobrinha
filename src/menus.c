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
	wclear(inner);
	makeborder(inner);

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

	char *opt_options[] = {"Layout: ", "Timer: ", "Mapa: ", "Speed:", "Voltar"};
	int opt_amt = 5;

	char *layout_options[] = {"QWERTY", "Colemak"};
	char *time_options[] = {"Normal", "00:30", "01:00", "03:00", "05:00"};
	char *map_options[] = {"Com Borda", "Sem Borda"};
	char *speed_options[] = {"Slow", "Normal", "Fast", "INSANE"};

	for(int i = 0; i < opt_amt; i++) {
		if(i != 4) {
			mvwprintw(inner, 3 + 2*i, 8, opt_options[i]);
		} else {
			mvwprintw(inner, 3 + 2*i, (32 - strlenunicode(opt_options[i]))/2, opt_options[i]);
		}
	}

	int op_teclado, op_tempo, op_mapa, op_speed; 
	loadoptions(&op_teclado, &op_tempo, &op_mapa, &op_speed);

	mvwprintw(inner, 3, 16, layout_options[op_teclado]);
	mvwprintw(inner, 5, 16, time_options[op_tempo]);
	mvwprintw(inner, 7, 16, map_options[op_mapa]);
	mvwprintw(inner, 9, 16, speed_options[op_speed]);

	int selected = 0, selX, HLsize;

	for(;;) {
		if(selected == 0) {
			HLsize = strlenunicode(layout_options[op_teclado]);
			selX = 16;
		} else if(selected == 1) {
			HLsize = strlenunicode(time_options[op_tempo]);
			selX = 16;
		} else if(selected == 2) {
			HLsize = strlenunicode(map_options[op_mapa]);
			selX = 16;
		} else if(selected == 3) {
			HLsize = strlenunicode(speed_options[op_speed]);
			selX = 16;
		} else if(selected == 4) {
			selX = 1 + (30 - strlenunicode(opt_options[3]))/2;
			HLsize = 6;
		}

		for(int i = 0; i < opt_amt; i++) {
			mvwchgat(inner, 3 + 2*i, 2, 25, A_NORMAL, GREEN, NULL);
		}
		mvwchgat(inner, 3 + selected*2, selX, HLsize, A_STANDOUT, GREEN, NULL);

		wrefresh(inner);

		int g = wgetch(inner);
		if(selected != 4) {
			mvwprintw(inner, 3 + 2*selected, 16, "          ");
		}
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
			if(selected == 0) {
				if(op_teclado > 0) {
					op_teclado--;
				} else if(op_teclado == 0){
					op_teclado = 1;
				}
				setletters(op_teclado);
			} else if(selected == 1) {
				if(op_tempo > 0) {
					op_tempo--;
				} else if(op_tempo == 0){
					op_tempo = 4;
				}
			} else if(selected == 2) {
				if(op_mapa > 0) {
					op_mapa--;
				} else if(op_mapa == 0){
					op_mapa = 1;
				}
			} else if(selected == 3) {
				if(op_speed > 0) {
					op_speed--;
				} else if(op_speed == 0){
					op_speed = 3;
				}
			}
		} else if(g == KEY_RIGHT || g == ltrrght) {
			if(selected == 0) {
				op_teclado = (op_teclado + 1) % 2;
				setletters(op_teclado);
			} else if(selected == 1) {
				op_tempo = (op_tempo + 1) % 5;
			} else if(selected == 2) {
				op_mapa = (op_mapa + 1) % 2;
			} else if(selected == 3) {
				op_speed = (op_speed + 1) % 4;
			}
		}
		// Condicao para salvar as opcoes e sair do menu
		else if((g == ' ' || g == '\n') && selected == 4) {
			saveoptions(op_teclado, op_tempo, op_mapa, op_speed);
			return;
		}

		// Refaz as opcoes caso seja necessario corrigir
		mvwprintw(inner, 3, 16, layout_options[op_teclado]);
		mvwprintw(inner, 5, 16, time_options[op_tempo]);
		mvwprintw(inner, 7, 16, map_options[op_mapa]);
		mvwprintw(inner, 9, 16, speed_options[op_speed]);
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
