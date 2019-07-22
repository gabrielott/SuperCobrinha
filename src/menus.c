#include <ncurses.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>

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

	typedef struct Selector {
		char *title;
		char **options;
		int optamt;
		int selected;
	} Selector;

	Selector *newselector(char *title, char **options, int optamt) {
		Selector *s = malloc(sizeof(Selector));
		s->title = title;
		s->options = options;
		s->optamt = optamt;
		s->selected = 0;
		return s;
	}

	// Para criar uma nova opcao, crie um novo Selector, aumente optamt em uma
	// unidade e adicione o novo Selector no array options. A ordem das opcoes
	// dentro do array determina a ordem como elas aparecerao no menu.

	char *layout_op[] = {"QWERTY", "Colemak"};
	Selector *layout = newselector("Layout:", layout_op, 2);

	char *timer_op[] = {"Não", "30s", "1min", "3min", "5min"};
	Selector *timer = newselector("Timer:", timer_op, 4);

	char *border_op[] = {"Sim", "Não"};
	Selector *border = newselector("Borda:", border_op, 2);

	char *speed_op[] = {"Slow", "Normal", "Fast", "INSANE"};
	Selector *speed = newselector("Speed:", speed_op, 4);

	char *color_op[] = {"Classic", "Scarlet"};
	Selector *color = newselector("Cor:", color_op, 2);

	// Carregamento das opcoes de mapa
	DIR *dir = opendir("maps");
	struct dirent *ent;

	int mapnum = 0;
	while((ent = readdir(dir)) != NULL) {
		if(ent->d_type == DT_DIR) continue;

		Map *m = newmap(ent->d_name);
		if(m != NULL) mapnum++;
		free(m);
	}

	rewinddir(dir);

	char **map_op = malloc(mapnum * sizeof(char *));
	char **filenames = malloc(mapnum * sizeof(char *));
	int i = 0;
	while((ent = readdir(dir)) != NULL) {
		if(ent->d_type == DT_DIR) continue;

		Map *m = newmap(ent->d_name);
		if(m == NULL) continue;

		char *n = malloc((strlen(ent->d_name) + 1) * sizeof(char));
		strcpy(n, ent->d_name);
		filenames[strcmp(ent->d_name, "default") == 0 ? 0 : ++i] = n;

		char *c = malloc((strlen(m->name) + 1) * sizeof(char));
		strcpy(c, m->name);
		map_op[strcmp(ent->d_name, "default") == 0 ? 0 : i] = c;

		free(m);
	}
		
	closedir(dir);

	Selector *map = newselector("Mapa:", map_op, mapnum);

	// Desenho inicial das opcoes, mude o array options e o int optamt para
	// adicionar novas opcoes
	Selector *options[] = {layout, timer, border, speed, color, map};

	int optamt = 6;
	int selected = 0;

	int biggest = 0;
	for(int i = 0; i < optamt; i++) {
		if(strlenunicode(options[i]->title) + 1 > biggest) biggest = strlenunicode(options[i]->title) + 1;
	}

	for(int i = 0; i < optamt; i++) {
		mvwprintw(inner, 1 + 2 * i, 8, options[i]->title);
		mvwprintw(inner, 1 + 2 * i, 8 + biggest, options[i]->options[options[i]->selected]);
	}

	char *backtext = "Salvar e voltar";
	mvwprintw(inner, 1 + 2 * optamt, (maxinx - strlenunicode(backtext)) / 2, backtext);

	wrefresh(inner);

	// A selecao inicial de cada opcao deve ser feita aqui. Modifique o atributo selected
	// da sua instancia de Selector. Caso isso nao seja feito, o Selector tera a primeira
	// opcao selecionada por padrao.
	
	int op_layout, op_timer, op_border, op_speed;
	loadoptions(&op_layout, &op_timer, &op_border, &op_speed);

	options[0]->selected = op_layout;
	options[1]->selected = op_timer;
	options[2]->selected = op_border;
	options[3]->selected = op_speed;
	options[4]->selected = GAMECORES.ID;

	// A funcao onsave() eh chamada assim que o usuario seleciona a opcao de salvar no
	// menu opcoes. Tudo que precisa acontecer para que sua opcao tenha efeito deve ser
	// feito aqui dentro. Voce pode acessar os atributos do seu Selector por meio do 
	// argumento options.

	void onsave(Selector **options) {
		setletters(options[0]->selected);
		GAMECORES = setscheme(options[4]->selected);

		saveoptions(options[0]->selected, options[1]->selected, options[2]->selected, options[3]->selected);
		savescheme();
		redraw_all();

		gamemap = newmap(filenames[options[5]->selected]);
		if(gamemap == NULL) wclear(wmain);

		// Desalocando memoria usada
		for(int i = 0; i < optamt; i++) {
			free(options[i]);
		}

		for(int i = 0; i < mapnum; i++) {
			free(map_op[i]);
			free(filenames[i]);
		}

		free(map_op);
		free(filenames);
	}

	// Loop que controla a parte visual do menu. Nao eh necessario alterar nada aqui dentro
	// para adicionar, remover ou mudar os efeitos de uma opcao, tudo isso pode ser feito
	// onde os comentarios anteriores indicam.
	
	for(;;) {
		for(int i = 0; i < optamt + 1; i++) {
			if(i == optamt) {
				if(i == selected) {
					mvwchgat(inner, 1 + 2 * optamt, (maxinx - strlenunicode(backtext)) / 2, strlenunicode(backtext), A_STANDOUT, GAMECORES.corMenuHL, NULL);
				} else {
					mvwchgat(inner, 1 + 2 * optamt, (maxinx - strlenunicode(backtext)) / 2, strlenunicode(backtext), A_NORMAL, GAMECORES.corMenu, NULL);
				}
				continue;
			}

			char formatted[20];
			strcpy(formatted, options[i]->options[options[i]->selected]);
			strcat(formatted, "     ");

			mvwprintw(inner, 1 + 2 * i, 8 + biggest, formatted);

			if(i == selected) {
				mvwchgat(inner, 1 + 2 * i, 8 + biggest, strlenunicode(options[i]->options[options[i]->selected]), A_STANDOUT, GAMECORES.corMenuHL, NULL);
			} else {
				mvwchgat(inner, 1 + 2 * i, 8 + biggest, strlenunicode(options[i]->options[options[i]->selected]), A_NORMAL, GAMECORES.corMenu, NULL);
			}
		}

		wrefresh(inner);

		int g = wgetch(inner);
		if((g == KEY_UP || g == ltrup) && selected > 0) {
			selected--;
		} else if((g == KEY_UP || g == ltrup) && selected == 0) {
			selected = optamt;
		} else if((g == KEY_DOWN || g == ltrdwn) && selected < optamt) {
			selected++;
		} else if((g == KEY_DOWN || g == ltrdwn) && selected == optamt) {
			selected = 0;
		} else if((g == ' ' || g == '\n') && selected == optamt) {
			onsave(options);
			break;
		} else if(g == ' ' || g == '\n' || g == KEY_RIGHT || g == ltrrght) {
			options[selected]->selected = options[selected]->selected == options[selected]->optamt - 1 ? 0 : options[selected]->selected + 1;
		} else if(g == KEY_LEFT || g == ltrlft) {
			options[selected]->selected = options[selected]->selected == 0 ? options[selected]->optamt - 1 : options[selected]->selected - 1;
		}
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
