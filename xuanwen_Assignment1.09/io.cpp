#include <unistd.h>
#include <ncurses.h>
#include <cctype>
#include <cstdlib>
#include <climits>
#include <string>
#include <cstring>

#include "io.h"
#include "character.h"
#include "poke327.h"
#include "pokemon.h"

#define TRAINER_LIST_FIELD_WIDTH 46

typedef struct io_message {
  /* Will print " --more-- " at end of line when another message follows. *
   * Leave 10 extra spaces for that.                                      */
  char msg[71];
  struct io_message *next;
} io_message_t;

static io_message_t *io_head, *io_tail;

void io_init_terminal(void)
{
  initscr();
  raw();
  noecho();
  curs_set(0);
  keypad(stdscr, TRUE);
  start_color();
  init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
  init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
  init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
  init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
  init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
  init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
}

void io_reset_terminal(void)
{
  endwin();

  while (io_head) {
    io_tail = io_head;
    io_head = io_head->next;
    free(io_tail);
  }
  io_tail = NULL;
}

void io_queue_message(const char *format, ...)
{
  io_message_t *tmp;
  va_list ap;

  if (!(tmp = (io_message_t *) malloc(sizeof (*tmp)))) {
    perror("malloc");
    exit(1);
  }

  tmp->next = NULL;

  va_start(ap, format);

  vsnprintf(tmp->msg, sizeof (tmp->msg), format, ap);

  va_end(ap);

  if (!io_head) {
    io_head = io_tail = tmp;
  } else {
    io_tail->next = tmp;
    io_tail = tmp;
  }
}

static void io_print_message_queue(uint32_t y, uint32_t x)
{
  while (io_head) {
    io_tail = io_head;
    attron(COLOR_PAIR(COLOR_CYAN));
    mvprintw(y, x, "%-80s", io_head->msg);
    attroff(COLOR_PAIR(COLOR_CYAN));
    io_head = io_head->next;
    if (io_head) {
      attron(COLOR_PAIR(COLOR_CYAN));
      mvprintw(y, x + 70, "%10s", " --more-- ");
      attroff(COLOR_PAIR(COLOR_CYAN));
      refresh();
      getch();
    }
    free(io_tail);
  }
  io_tail = NULL;
}

/**************************************************************************
 * Compares trainer distances from the PC according to the rival distance *
 * map.  This gives the approximate distance that the PC must travel to   *
 * get to the trainer (doesn't account for crossing buildings).  This is  *
 * not the distance from the NPC to the PC unless the NPC is a rival.     *
 *                                                                        *
 * Not a bug.                                                             *
 **************************************************************************/
static int compare_trainer_distance(const void *v1, const void *v2)
{
  const character *const *c1 = (const character * const *) v1;
  const character *const *c2 = (const character * const *) v2;

  return (world.rival_dist[(*c1)->pos[dim_y]][(*c1)->pos[dim_x]] -
          world.rival_dist[(*c2)->pos[dim_y]][(*c2)->pos[dim_x]]);
}

static character *io_nearest_visible_trainer()
{
  character **c, *n;
  uint32_t x, y, count;

  c = (character **) malloc(world.cur_map->num_trainers * sizeof (*c));

  /* Get a linear list of trainers */
  for (count = 0, y = 1; y < MAP_Y - 1; y++) {
    for (x = 1; x < MAP_X - 1; x++) {
      if (world.cur_map->cmap[y][x] && world.cur_map->cmap[y][x] !=
          &world.pc) {
        c[count++] = world.cur_map->cmap[y][x];
      }
    }
  }

  /* Sort it by distance from PC */
  qsort(c, count, sizeof (*c), compare_trainer_distance);

  n = c[0];

  free(c);

  return n;
}

void io_display()
{
  uint32_t y, x;
  character *c;

  clear();
  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      if (world.cur_map->cmap[y][x]) {
        mvaddch(y + 1, x, world.cur_map->cmap[y][x]->symbol);
      } else {
        switch (world.cur_map->map[y][x]) {
        case ter_boulder:
          attron(COLOR_PAIR(COLOR_MAGENTA));
          mvaddch(y + 1, x, BOULDER_SYMBOL);
          attroff(COLOR_PAIR(COLOR_MAGENTA));
          break;
        case ter_mountain:
          attron(COLOR_PAIR(COLOR_MAGENTA));
          mvaddch(y + 1, x, MOUNTAIN_SYMBOL);
          attroff(COLOR_PAIR(COLOR_MAGENTA));
          break;
        case ter_tree:
          attron(COLOR_PAIR(COLOR_GREEN));
          mvaddch(y + 1, x, TREE_SYMBOL);
          attroff(COLOR_PAIR(COLOR_GREEN));
          break;
        case ter_forest:
          attron(COLOR_PAIR(COLOR_GREEN));
          mvaddch(y + 1, x, FOREST_SYMBOL);
          attroff(COLOR_PAIR(COLOR_GREEN));
          break;
        case ter_path:
        case ter_bailey:
          attron(COLOR_PAIR(COLOR_YELLOW));
          mvaddch(y + 1, x, PATH_SYMBOL);
          attroff(COLOR_PAIR(COLOR_YELLOW));
          break;
        case ter_gate:
          attron(COLOR_PAIR(COLOR_YELLOW));
          mvaddch(y + 1, x, GATE_SYMBOL);
          attroff(COLOR_PAIR(COLOR_YELLOW));
          break;
        case ter_mart:
          attron(COLOR_PAIR(COLOR_BLUE));
          mvaddch(y + 1, x, POKEMART_SYMBOL);
          attroff(COLOR_PAIR(COLOR_BLUE));
          break;
        case ter_center:
          attron(COLOR_PAIR(COLOR_RED));
          mvaddch(y + 1, x, POKEMON_CENTER_SYMBOL);
          attroff(COLOR_PAIR(COLOR_RED));
          break;
        case ter_grass:
          attron(COLOR_PAIR(COLOR_GREEN));
          mvaddch(y + 1, x, TALL_GRASS_SYMBOL);
          attroff(COLOR_PAIR(COLOR_GREEN));
          break;
        case ter_clearing:
          attron(COLOR_PAIR(COLOR_GREEN));
          mvaddch(y + 1, x, SHORT_GRASS_SYMBOL);
          attroff(COLOR_PAIR(COLOR_GREEN));
          break;
        case ter_water:
          attron(COLOR_PAIR(COLOR_CYAN));
          mvaddch(y + 1, x, WATER_SYMBOL);
          attroff(COLOR_PAIR(COLOR_CYAN));
          break;
        default:
 /* Use zero as an error symbol, since it stands out somewhat, and it's *
  * not otherwise used.                                                 */
          attron(COLOR_PAIR(COLOR_CYAN));
          mvaddch(y + 1, x, ERROR_SYMBOL);
          attroff(COLOR_PAIR(COLOR_CYAN)); 
       }
      }
    }
  }

  mvprintw(23, 1, "PC position is (%2d,%2d) on map %d%cx%d%c.",
           world.pc.pos[dim_x],
           world.pc.pos[dim_y],
           abs(world.cur_idx[dim_x] - (WORLD_SIZE / 2)),
           world.cur_idx[dim_x] - (WORLD_SIZE / 2) >= 0 ? 'E' : 'W',
           abs(world.cur_idx[dim_y] - (WORLD_SIZE / 2)),
           world.cur_idx[dim_y] - (WORLD_SIZE / 2) <= 0 ? 'N' : 'S');
  mvprintw(22, 1, "%d known %s.", world.cur_map->num_trainers,
           world.cur_map->num_trainers > 1 ? "trainers" : "trainer");
  mvprintw(22, 30, "Nearest visible trainer: ");
  if ((c = io_nearest_visible_trainer())) {
    attron(COLOR_PAIR(COLOR_RED));
    mvprintw(22, 55, "%c at vector %d%cx%d%c.",
             c->symbol,
             abs(c->pos[dim_y] - world.pc.pos[dim_y]),
             ((c->pos[dim_y] - world.pc.pos[dim_y]) <= 0 ?
              'N' : 'S'),
             abs(c->pos[dim_x] - world.pc.pos[dim_x]),
             ((c->pos[dim_x] - world.pc.pos[dim_x]) <= 0 ?
              'W' : 'E'));
    attroff(COLOR_PAIR(COLOR_RED));
  } else {
    attron(COLOR_PAIR(COLOR_BLUE));
    mvprintw(22, 55, "NONE.");
    attroff(COLOR_PAIR(COLOR_BLUE));
  }

  io_print_message_queue(0, 0);

  refresh();
}

uint32_t io_teleport_pc(pair_t dest)
{
  /* Just for fun. And debugging.  Mostly debugging. */

  do {
    dest[dim_x] = rand_range(1, MAP_X - 2);
    dest[dim_y] = rand_range(1, MAP_Y - 2);
  } while (world.cur_map->cmap[dest[dim_y]][dest[dim_x]]                  ||
           move_cost[char_pc][world.cur_map->map[dest[dim_y]]
                                                [dest[dim_x]]] ==
             DIJKSTRA_PATH_MAX                                            ||
           world.rival_dist[dest[dim_y]][dest[dim_x]] < 0);

  return 0;
}

static void io_scroll_trainer_list(char (*s)[TRAINER_LIST_FIELD_WIDTH],
                                   uint32_t count)
{
  uint32_t offset;
  uint32_t i;

  offset = 0;

  while (1) {
    for (i = 0; i < 13; i++) {
      mvprintw(i + 6, 19, " %-40s ", s[i + offset]);
    }
    switch (getch()) {
    case KEY_UP:
      if (offset) {
        offset--;
      }
      break;
    case KEY_DOWN:
      if (offset < (count - 13)) {
        offset++;
      }
      break;
    case 27:
      return;
    }

  }
}

static void io_list_trainers_display(npc **c, uint32_t count)
{
  uint32_t i;
  char (*s)[TRAINER_LIST_FIELD_WIDTH]; /* pointer to array of 40 char */

  s = (char (*)[TRAINER_LIST_FIELD_WIDTH]) malloc(count * sizeof (*s));

  mvprintw(3, 19, " %-40s ", "");
  /* Borrow the first element of our array for this string: */
  snprintf(s[0], TRAINER_LIST_FIELD_WIDTH, "You know of %d trainers:", count);
  mvprintw(4, 19, " %-40s ", *s);
  mvprintw(5, 19, " %-40s ", "");

  for (i = 0; i < count; i++) {
    snprintf(s[i], TRAINER_LIST_FIELD_WIDTH, "%16s %c: %2d %s by %2d %s",
             char_type_name[c[i]->ctype],
             c[i]->symbol,
             abs(c[i]->pos[dim_y] - world.pc.pos[dim_y]),
             ((c[i]->pos[dim_y] - world.pc.pos[dim_y]) <= 0 ?
              "North" : "South"),
             abs(c[i]->pos[dim_x] - world.pc.pos[dim_x]),
             ((c[i]->pos[dim_x] - world.pc.pos[dim_x]) <= 0 ?
              "West" : "East"));
    if (count <= 13) {
      /* Handle the non-scrolling case right here. *
       * Scrolling in another function.            */
      mvprintw(i + 6, 19, " %-40s ", s[i]);
    }
  }

  if (count <= 13) {
    mvprintw(count + 6, 19, " %-40s ", "");
    mvprintw(count + 7, 19, " %-40s ", "Hit escape to continue.");
    while (getch() != 27 /* escape */)
      ;
  } else {
    mvprintw(19, 19, " %-40s ", "");
    mvprintw(20, 19, " %-40s ",
             "Arrows to scroll, escape to continue.");
    io_scroll_trainer_list(s, count);
  }

  free(s);
}

static void io_list_trainers()
{
  npc **c;
  uint32_t x, y, count;

  c = (npc **) malloc(world.cur_map->num_trainers * sizeof (*c));

  /* Get a linear list of trainers */
  for (count = 0, y = 1; y < MAP_Y - 1; y++) {
    for (x = 1; x < MAP_X - 1; x++) {
      if (world.cur_map->cmap[y][x] && world.cur_map->cmap[y][x] !=
          &world.pc) {
        c[count++] = dynamic_cast <npc *> (world.cur_map->cmap[y][x]);
      }
    }
  }

  /* Sort it by distance from PC */
  qsort(c, count, sizeof (*c), compare_trainer_distance);

  /* Display it */
  io_list_trainers_display(c, count);
  free(c);

  /* And redraw the map */
  io_display();
}

void io_pokemart()
{
  //mvprintw(0, 0, "Welcome to the Pokemart.  Could I interest you in some Pokeballs?");
  mvprintw(0,0, "Your supplies have been restored!");
  refresh();
  for(int i=0; i<3; i++) {
	world.pc.supplies[i] = 5;
  }
  getch();
}

void io_pokemon_center()
{
  //mvprintw(0, 0, "Welcome to the Pokemon Center.  How can Nurse Joy assist you?");
  refresh();
  mvprintw(0,0, "Your Pokemons are all fully healed!");
  for(int i=0; i<6 && world.pc.buddy[i] ; i++) {
	world.pc.buddy[i]->set_health(world.pc.buddy[i]->get_hp());
  }
  getch();
}

int io_use_item(int item) {	// 0:revives, 1:potions, 2:Poke ́balls
	char input;
	if(item == 0) {
		if(world.pc.supplies[0] > 0) {
			mvprintw(11,0, "Choose a Pokemon to revive:	(c) cancel");
			mvprintw(12,1, "(1) %s", world.pc.buddy[0]->get_species());
			if(world.pc.buddy[1] != NULL) {
				mvprintw(13,1, "(2) %s", world.pc.buddy[1]->get_species());
			} else if(world.pc.buddy[2] != NULL) {
				mvprintw(14,1, "(3) %s", world.pc.buddy[2]->get_species());
			} else if(world.pc.buddy[3] != NULL) {
				mvprintw(15,1, "(4) %s", world.pc.buddy[3]->get_species());
			} else if(world.pc.buddy[4] != NULL) {
				mvprintw(16,1, "(5) %s", world.pc.buddy[4]->get_species());
			} else if(world.pc.buddy[5] != NULL) {
				mvprintw(17,1, "(6) %s", world.pc.buddy[5]->get_species());
			}
		} else {	// No revive
			return 1;
		}

		input = getch();
		pokemon *pkm;
		if(input == '1') {
			pkm = world.pc.buddy[0];
		} else if (input == '2') {
			pkm = world.pc.buddy[1];
		} else if (input == '3') {
			pkm = world.pc.buddy[2];
		} else if (input == '4') {
			pkm = world.pc.buddy[3];
		} else if (input == '5') {
			pkm = world.pc.buddy[4];
		} else if (input == '6') {
			pkm = world.pc.buddy[5];
		} else {
			return 1;
		}

		if(pkm == NULL || pkm->get_health() > 0) {
			return 1;
		}

		pkm->set_health(pkm->get_hp()/2);
		world.pc.supplies[0]--;
		return 0;
	}
	else if(item == 1) {
		if(world.pc.supplies[1] > 0) {
			mvprintw(11,0, "Choose a Pokemon to heal:	(c) cancel");
			mvprintw(12,1, "(1) %s", world.pc.buddy[0]->get_species());
			if(world.pc.buddy[1] != NULL) {
				mvprintw(13,1, "(2) %s", world.pc.buddy[1]->get_species());
			} else if(world.pc.buddy[2] != NULL) {
				mvprintw(14,1, "(3) %s", world.pc.buddy[2]->get_species());
			} else if(world.pc.buddy[3] != NULL) {
				mvprintw(15,1, "(4) %s", world.pc.buddy[3]->get_species());
			} else if(world.pc.buddy[4] != NULL) {
				mvprintw(16,1, "(5) %s", world.pc.buddy[4]->get_species());
			} else if(world.pc.buddy[5] != NULL) {
				mvprintw(17,1, "(6) %s", world.pc.buddy[5]->get_species());
			}
		} else {	// No potion
			return -1;
		}

		char input = getch();
		pokemon *pkm;
		if(input == '1') {
			pkm = world.pc.buddy[0];
		} else if (input == '2') {
			pkm = world.pc.buddy[1];
		} else if (input == '3') {
			pkm = world.pc.buddy[2];
		} else if (input == '4') {
			pkm = world.pc.buddy[3];
		} else if (input == '5') {
			pkm = world.pc.buddy[4];
		} else if (input == '6') {
			pkm = world.pc.buddy[5];
		} else {
			return 1;
		}

		if(pkm == NULL || pkm->get_health() == pkm->get_hp()) {
			return 1;
		}

		pkm->set_health(std::min((pkm->get_health()+20), (pkm->get_hp())));
		world.pc.supplies[1]--;
		return 0;
	}
	else {
		if (world.pc.supplies[2] > 0) {
			if(world.pc.buddy[5] != NULL) {
				return 0;
			}

			for(int i=1; i<6; i++) {
				if(world.pc.buddy[i] == NULL) {
					world.pc.supplies[2]--;
					return i;
				}
			}
		} else {
			return -1;
		}
	}
	return 0;
}


int io_swtich(pokemon **pcPokemon) {
	mvprintw(11, 0, "Which Pokemon to swtich?	(c) cancel");
	mvprintw(12,1, "(1) %s", world.pc.buddy[0]->get_species());
	if(world.pc.buddy[1] != NULL) {
		mvprintw(13,1, "(2) %s", world.pc.buddy[1]->get_species());
	} else if(world.pc.buddy[2] != NULL) {
		mvprintw(14,1, "(3) %s", world.pc.buddy[2]->get_species());
	} else if(world.pc.buddy[3] != NULL) {
		mvprintw(15,1, "(4) %s", world.pc.buddy[3]->get_species());
	} else if(world.pc.buddy[4] != NULL) {
		mvprintw(16,1, "(5) %s", world.pc.buddy[4]->get_species());
	} else if(world.pc.buddy[5] != NULL) {
		mvprintw(17,1, "(6) %s", world.pc.buddy[5]->get_species());
	}

	char input = getch(); 
	pokemon *pkm;
	if(input == '1') {
		pkm = world.pc.buddy[0];
	} else if (input == '2') {
		pkm = world.pc.buddy[1];
	} else if (input == '3') {
		pkm = world.pc.buddy[2];
	} else if (input == '4') {
		pkm = world.pc.buddy[3];
	} else if (input == '5') {
		pkm = world.pc.buddy[4];
	} else if (input == '6') {
		pkm = world.pc.buddy[5];
	} else {
		return 1;
	}

	if (pkm == NULL || pkm->get_health() == 0 || *pcPokemon == pkm) {
		return 1;
	}
	*pcPokemon = pkm;
	return 0;
}

void io_attack(pokemon *opponent, pokemon *pcPokemon, int move) {
	int opp_prior = opponent->get_move_priority(0);
	int opp_speed = opponent->get_speed();
	int pc_prior = pcPokemon->get_move_priority(move);
	int pc_speed = pcPokemon->get_speed();

	bool pcMove;
	if(pc_prior > opp_prior) {
		pcMove = true;
	} else if(pc_prior < opp_prior) {
		pcMove = false;
	} else {
		if(pc_speed > opp_speed) {
			pcMove = true;
		} else if(pc_speed < opp_speed) {
			pcMove = false;
		} else {
			if(rand()%2 == 0) {
				pcMove = true;
			}else {
				pcMove = false;
			}
		}
	}

	int damage;
	if(pcMove) {
		if((rand()%100) < pcPokemon->get_move_accuracy(move)) {
			damage = ((((((2*pcPokemon->get_level())/5)+2)*pcPokemon->get_move_power(move) * (pcPokemon->get_atk() / opponent->get_def()))/50)+2);
			opponent->set_health(std::max((opponent->get_health()-damage), 0));
		}

		if(opponent->get_health() == 0) {
			return;
		}

		if((rand()%100) < opponent->get_move_accuracy(0)) {
			damage = ((((((2*opponent->get_level())/5)+2)*opponent->get_move_power(0) * (opponent->get_atk() / pcPokemon->get_def()))/50)+2);
			pcPokemon->set_health(std::max((pcPokemon->get_health()-damage), 0));
		}
	} else {
		if((rand()%100) < opponent->get_move_accuracy(0)) {
			damage = ((((((2*opponent->get_level())/5)+2)*opponent->get_move_power(0) * (opponent->get_atk() / pcPokemon->get_def()))/50)+2);
			pcPokemon->set_health(std::max((pcPokemon->get_health()-damage), 0));
		}

		if(pcPokemon->get_health() == 0) {
			return;
		}

		if((rand()%100) < pcPokemon->get_move_accuracy(move)) {
			damage = ((((((2*pcPokemon->get_level())/5)+2)*pcPokemon->get_move_power(move) * (pcPokemon->get_atk() / opponent->get_def()))/50)+2);
			opponent->set_health(std::max((opponent->get_health()-damage), 0));
		}
	}
}

// void io_battle(character *aggressor, character *defender)
// {
//   std::string s;
//   npc *n = (npc *) ((aggressor == &world.pc) ? defender : aggressor);
//   int i;

//   if (!n->buddy[1]) {
//     s = "My pokemon is " + std::string(n->buddy[0]->get_species());
//   } else {
//     s = "My pokemon are " + std::string(n->buddy[0]->get_species());
//   }

//   for (i = 1; i < 6 && n->buddy[i]; i++) {
//     s += ", ";
//     if (i == 4 || !n->buddy[i + 1]) {
//       s += "and ";
//     }
//     s += n->buddy[i]->get_species();
//   }
    
//   s += ".";

//   io_queue_message("%s", s.c_str());

//   n->defeated = 1;
//   if (n->ctype == char_hiker || n->ctype == char_rival) {
//     n->mtype = move_wander;
//   }
// }

void io_battle(character *aggressor, character *defender) {
	npc *n = (npc *) ((aggressor == &world.pc) ? defender : aggressor);
	bool battleCont = true;
	int turn = 1;
	int npcPokeIdx = 0;
	pokemon *npcPokemon = n->buddy[npcPokeIdx];
	pokemon *pcPokemon;
	for(int i=0; i<6; i++) {
		if(world.pc.buddy[i]->get_health() != 0) {
			pcPokemon = world.pc.buddy[i];
			break;
		}
	}

	do{
		do{
			clear();

			mvprintw(0,0, "Your Pokemon: %s -- HP: %d", pcPokemon->get_species(), pcPokemon->get_health());
			mvprintw(1,0, "Opponent's Pokemon: %s -- HP: %d", npcPokemon->get_species(), npcPokemon->get_health());
			// PC moves
			mvprintw(3,0, "Make a move:");
			mvprintw(4,1, "(1) %s", pcPokemon->get_move(0));
			if(strcmp(pcPokemon->get_move(1), "") != 0) {
				mvprintw(5,1, "(2) %s", pcPokemon->get_move(1));
			}
			if(strcmp(pcPokemon->get_move(2), "") != 0) {
				mvprintw(6,1, "(3) %s", pcPokemon->get_move(2));
			}
			if(strcmp(pcPokemon->get_move(3), "") != 0) {
				mvprintw(7,1, "(4) %s", pcPokemon->get_move(3));
			}

			// PC supplies
			mvprintw(3, 22, "Choose an item:");
			mvprintw(4,23, "(r) Revives -- %d", world.pc.supplies[0]);
			mvprintw(5,23, "(p) Potions -- %d", world.pc.supplies[1]);

			// Switch Pokemon
			mvprintw(9,1, "(s) Switch Pokemon");
			mvprintw(10,1, "(f) Flee");

			char action;
			switch(action = getch()) {
				case '1':
					io_attack(npcPokemon, pcPokemon, 0);
					turn = 0;
					break;

				case '2':
				if(strcmp(pcPokemon->get_move(1),"") != 0){
					io_attack(npcPokemon, pcPokemon, 1);
					turn = 0;
				} else {
					turn = 1;
				}
				break;

				case '3':
				if(strcmp(pcPokemon->get_move(2),"") != 0){
					io_attack(npcPokemon, pcPokemon, 2);
					turn = 0;
				} else {
					turn = 1;
				}
				break;

				case '4':
				if(strcmp(pcPokemon->get_move(3),"") != 0){
					io_attack(npcPokemon, pcPokemon, 3);
					turn = 0;
				} else {
					turn = 1;
				}
				break;

				case 'r':
					turn = io_use_item(0);
					if(turn == 0) {
						int damage;
						if((rand()%100) < npcPokemon->get_move_accuracy(0)) {
							damage = ((((((2*npcPokemon->get_level())/5)+2)*npcPokemon->get_move_power(0) * (npcPokemon->get_atk() / pcPokemon->get_def()))/50)+2);
							pcPokemon->set_health(std::max((pcPokemon->get_health()-damage), 0));
						}		
					}
				break;

				case 'p':
					turn = io_use_item(1);
					if(turn == 0) {
						int damage;
						if((rand()%100) < npcPokemon->get_move_accuracy(0)) {
							damage = ((((((2*npcPokemon->get_level())/5)+2)*npcPokemon->get_move_power(0) * (npcPokemon->get_atk() / pcPokemon->get_def()))/50)+2);
							pcPokemon->set_health(std::max((pcPokemon->get_health()-damage), 0));
						}		
					}
				break;

				case 's':
					turn = io_swtich(&pcPokemon);
					if(turn == 0) {
						int damage;
						if((rand()%100) < npcPokemon->get_move_accuracy(0)) {
							damage = ((((((2*npcPokemon->get_level())/5)+2)*npcPokemon->get_move_power(0) * (npcPokemon->get_atk() / pcPokemon->get_def()))/50)+2);
							pcPokemon->set_health(std::max((pcPokemon->get_health()-damage), 0));
						}	
					}
				break;

				case 'f':
					if((rand()%100) < 95) {
						battleCont = false;
					} else {
						int damage;
						if((rand()%100) < npcPokemon->get_move_accuracy(0)) {
							damage = ((((((2*npcPokemon->get_level())/5)+2)*npcPokemon->get_move_power(0) * (npcPokemon->get_atk() / pcPokemon->get_def()))/50)+2);
							pcPokemon->set_health(std::max((pcPokemon->get_health()-damage), 0));
						}
					}
					turn = 0;
				break;

				default:
					turn = 1;
			}
		}while(turn);

		if(npcPokemon->get_health() == 0) {
			++npcPokeIdx;

			if(npcPokeIdx == 6 || n->buddy[npcPokeIdx] == NULL) {
				n->defeated = 1;
				if(n->ctype == char_hiker || n->ctype == char_rival) {
					n->mtype = move_wander;
				}
				battleCont = false;
			} else {
				npcPokemon = n->buddy[npcPokeIdx];
			}
		}

		if(pcPokemon->get_health() == 0) {
			for (int j=0; j<=6; j++) {
				if(j==6 || world.pc.buddy[j] == NULL) {
					world.quit = 2;
					battleCont = false;
					break;
				}

				if(world.pc.buddy[j]->get_health() != 0) {
					pcPokemon = world.pc.buddy[j];
					break;
				}
			}
		}
	}while(battleCont);
}

void io_bag() {
	bool close = false;
	do {
		int turn = 0;
		do {
			clear();
			mvprintw(0,0, "Supplies in your bag:");
			mvprintw(1,1, "(r) Revives -- %d", world.pc.supplies[0]);
			mvprintw(2,1, "(p) Potions -- %d", world.pc.supplies[1]);
			mvprintw(3,1, "(b) Pokeballs -- %d", world.pc.supplies[2]);

			mvprintw(0, 21, "Pokemons:");
			mvprintw(1,22, "(1) %s -- HP: %d/%d", world.pc.buddy[0]->get_species(), world.pc.buddy[0]->get_health(),world.pc.buddy[0]->get_hp());
			if(world.pc.buddy[1] != NULL) {
				mvprintw(2,22, "(2) %s -- HP: %d/%d", world.pc.buddy[1]->get_species(), world.pc.buddy[1]->get_health(),world.pc.buddy[1]->get_hp());
			}
			if(world.pc.buddy[2] != NULL) {
				mvprintw(3,22, "(3) %s -- HP: %d/%d", world.pc.buddy[2]->get_species(), world.pc.buddy[2]->get_health(),world.pc.buddy[2]->get_hp());
			} 
			if(world.pc.buddy[3] != NULL) {
				mvprintw(4,22, "(4) %s -- HP: %d/%d", world.pc.buddy[3]->get_species(), world.pc.buddy[3]->get_health(),world.pc.buddy[3]->get_hp());
			} 
			if(world.pc.buddy[4] != NULL) {
				mvprintw(5,22, "(5) %s -- HP: %d/%d", world.pc.buddy[4]->get_species(), world.pc.buddy[4]->get_health(),world.pc.buddy[4]->get_hp());
			} 
			if(world.pc.buddy[5] != NULL) {
				mvprintw(6,22, "(6) %s -- HP: %d/%d", world.pc.buddy[5]->get_species(), world.pc.buddy[5]->get_health(),world.pc.buddy[5]->get_hp());
			} 
			mvprintw(8,0, "(c) Close bag");

			char item;
			switch(item = getch()) {
				case 'r':
					turn = io_use_item(0);
					break;
				case 'p':
					turn = io_use_item(1);
					break;
				case 'c':
					close = true;
					turn = 0;
					break;
				default:
					turn = 1;
					break;
			}
		}while(turn);
	}while(!close);

	return;
}

uint32_t move_pc_dir(uint32_t input, pair_t dest)
{
  dest[dim_y] = world.pc.pos[dim_y];
  dest[dim_x] = world.pc.pos[dim_x];

  switch (input) {
  case 1:
  case 2:
  case 3:
    dest[dim_y]++;
    break;
  case 4:
  case 5:
  case 6:
    break;
  case 7:
  case 8:
  case 9:
    dest[dim_y]--;
    break;
  }
  switch (input) {
  case 1:
  case 4:
  case 7:
    dest[dim_x]--;
    break;
  case 2:
  case 5:
  case 8:
    break;
  case 3:
  case 6:
  case 9:
    dest[dim_x]++;
    break;
  case '>':
    if (world.cur_map->map[world.pc.pos[dim_y]][world.pc.pos[dim_x]] ==
        ter_mart) {
      io_pokemart();
    }
    if (world.cur_map->map[world.pc.pos[dim_y]][world.pc.pos[dim_x]] ==
        ter_center) {
      io_pokemon_center();
    }
    break;
  }

  if (world.cur_map->cmap[dest[dim_y]][dest[dim_x]]) {
    if (dynamic_cast<npc *> (world.cur_map->cmap[dest[dim_y]][dest[dim_x]]) &&
        ((npc *) world.cur_map->cmap[dest[dim_y]][dest[dim_x]])->defeated) {
      // Some kind of greeting here would be nice
      return 1;
    } else if ((dynamic_cast<npc *>
                (world.cur_map->cmap[dest[dim_y]][dest[dim_x]]))) {
      io_battle(&world.pc, world.cur_map->cmap[dest[dim_y]][dest[dim_x]]);
      // Not actually moving, so set dest back to PC position
      dest[dim_x] = world.pc.pos[dim_x];
      dest[dim_y] = world.pc.pos[dim_y];
    }
  }
  
  if (move_cost[char_pc][world.cur_map->map[dest[dim_y]][dest[dim_x]]] ==
      DIJKSTRA_PATH_MAX) {
    return 1;
  }

  return 0;
}

void io_teleport_world(pair_t dest)
{
  /* mvscanw documentation is unclear about return values.  I believe *
   * that the return value works the same way as scanf, but instead   *
   * of counting on that, we'll initialize x and y to out of bounds   *
   * values and accept their updates only if in range.                */
  int x = INT_MAX, y = INT_MAX;
  
  world.cur_map->cmap[world.pc.pos[dim_y]][world.pc.pos[dim_x]] = NULL;

  echo();
  curs_set(1);
  do {
    mvprintw(0, 0, "Enter x [-200, 200]:           ");
    refresh();
    mvscanw(0, 21, (char *) "%d", &x);
  } while (x < -200 || x > 200);
  do {
    mvprintw(0, 0, "Enter y [-200, 200]:          ");
    refresh();
    mvscanw(0, 21, (char *) "%d", &y);
  } while (y < -200 || y > 200);

  refresh();
  noecho();
  curs_set(0);

  x += 200;
  y += 200;

  world.cur_idx[dim_x] = x;
  world.cur_idx[dim_y] = y;

  new_map(1);
  io_teleport_pc(dest);
}

void io_handle_input(pair_t dest)
{
  uint32_t turn_not_consumed;
  int key;

  do {
    switch (key = getch()) {
    case '7':
    case 'y':
    case KEY_HOME:
      turn_not_consumed = move_pc_dir(7, dest);
      break;
    case '8':
    case 'k':
    case KEY_UP:
      turn_not_consumed = move_pc_dir(8, dest);
      break;
    case '9':
    case 'u':
    case KEY_PPAGE:
      turn_not_consumed = move_pc_dir(9, dest);
      break;
    case '6':
    case 'l':
    case KEY_RIGHT:
      turn_not_consumed = move_pc_dir(6, dest);
      break;
    case '3':
    case 'n':
    case KEY_NPAGE:
      turn_not_consumed = move_pc_dir(3, dest);
      break;
    case '2':
    case 'j':
    case KEY_DOWN:
      turn_not_consumed = move_pc_dir(2, dest);
      break;
    case '1':
    case 'b':
    case KEY_END:
      turn_not_consumed = move_pc_dir(1, dest);
      break;
    case '4':
    case 'h':
    case KEY_LEFT:
      turn_not_consumed = move_pc_dir(4, dest);
      break;
    case '5':
    case ' ':
    case '.':
    case KEY_B2:
      dest[dim_y] = world.pc.pos[dim_y];
      dest[dim_x] = world.pc.pos[dim_x];
      turn_not_consumed = 0;
      break;
    case '>':
      turn_not_consumed = move_pc_dir('>', dest);
      break;
    case 'Q':
      dest[dim_y] = world.pc.pos[dim_y];
      dest[dim_x] = world.pc.pos[dim_x];
      world.quit = 1;
      turn_not_consumed = 0;
      break;
      break;
    case 't':
      io_list_trainers();
      turn_not_consumed = 1;
      break;
    case 'p':
      /* Teleport the PC to a random place in the map.              */
      io_teleport_pc(dest);
      turn_not_consumed = 0;
      break;
   case 'f':
      /* Fly to any map in the world.                                */
      io_teleport_world(dest);
      turn_not_consumed = 0;
      break;    
    case 'q':
      /* Demonstrate use of the message queue.  You can use this for *
       * printf()-style debugging (though gdb is probably a better   *
       * option.  Not that it matters, but using this command will   *
       * waste a turn.  Set turn_not_consumed to 1 and you should be *
       * able to figure out why I did it that way.                   */
      io_queue_message("This is the first message.");
      io_queue_message("Since there are multiple messages, "
                       "you will see \"more\" prompts.");
      io_queue_message("You can use any key to advance through messages.");
      io_queue_message("Normal gameplay will not resume until the queue "
                       "is empty.");
      io_queue_message("Long lines will be truncated, not wrapped.");
      io_queue_message("io_queue_message() is variadic and handles "
                       "all printf() conversion specifiers.");
      io_queue_message("Did you see %s?", "what I did there");
      io_queue_message("When the last message is displayed, there will "
                       "be no \"more\" prompt.");
      io_queue_message("Have fun!  And happy printing!");
      io_queue_message("Oh!  And use 'Q' to quit!");

      dest[dim_y] = world.pc.pos[dim_y];
      dest[dim_x] = world.pc.pos[dim_x];
      turn_not_consumed = 0;
      break;
	
	case 'B':
		io_bag();
		turn_not_consumed = 1;
		io_display();
		break;

    default:
      /* Also not in the spec.  It's not always easy to figure out what *
       * key code corresponds with a given keystroke.  Print out any    *
       * unhandled key here.  Not only does it give a visual error      *
       * indicator, but it also gives an integer value that can be used *
       * for that key in this (or other) switch statements.  Printed in *
       * octal, with the leading zero, because ncurses.h lists codes in *
       * octal, thus allowing us to do reverse lookups.  If a key has a *
       * name defined in the header, you can use the name here, else    *
       * you can directly use the octal value.                          */
      mvprintw(0, 0, "Unbound key: %#o ", key);
      turn_not_consumed = 1;
    }
    refresh();
  } while (turn_not_consumed);
}

// void io_encounter_pokemon()
// {
//   pokemon *p;

//   p = new pokemon();

//   io_queue_message("%s%s%s: HP:%d ATK:%d DEF:%d SPATK:%d SPDEF:%d SPEED:%d %s",
//                    p->is_shiny() ? "*" : "", p->get_species(),
//                    p->is_shiny() ? "*" : "", p->get_hp(), p->get_atk(),
//                    p->get_def(), p->get_spatk(), p->get_spdef(),
//                    p->get_speed(), p->get_gender_string());
//   io_queue_message("%s's moves: %s %s", p->get_species(),
//                    p->get_move(0), p->get_move(1));

//   // Later on, don't delete if captured
//   delete p;
// }

void io_encounter_pokemon() {
	pokemon *wildPokemon = new pokemon();
	bool battleCont = true;
	int turn = 1;
	pokemon *pcPokemon;
	for(int i =0; i<6; i++) {
		if(world.pc.buddy[i]->get_health() != 0) {
			pcPokemon = world.pc.buddy[i];
			break;
		}
	}

	do {
		do {
			clear();

			mvprintw(0,0, "Your Pokemon: %s -- HP: %d", pcPokemon->get_species(), pcPokemon->get_health());
			mvprintw(1,0, "Wild Pokemon: %s -- HP: %d", wildPokemon->get_species(), wildPokemon->get_health());
			// PC moves
			mvprintw(3,0, "Make a move:");
			mvprintw(4,1, "(1) %s", pcPokemon->get_move(0));
			if(strcmp(pcPokemon->get_move(1), "") != 0) {
				mvprintw(5,1, "(2) %s", pcPokemon->get_move(1));
			}
			if(strcmp(pcPokemon->get_move(2), "") != 0) {
				mvprintw(6,1, "(3) %s", pcPokemon->get_move(2));
			}
			if(strcmp(pcPokemon->get_move(3), "") != 0) {
				mvprintw(7,1, "(4) %s", pcPokemon->get_move(3));
			}

			// PC supplies
			mvprintw(3, 22, "Choose an item:");
			mvprintw(4,23, "(r) Revives -- %d", world.pc.supplies[0]);
			mvprintw(5,23, "(p) Potions -- %d", world.pc.supplies[1]);
			mvprintw(6,23, "(b) Pokeballs -- %d", world.pc.supplies[2]);

			// Switch Pokemon or Flee
			mvprintw(9,1, "(s) Switch Pokemon");
			mvprintw(10,1, "(f) Flee");

			char action;
			int catchResult;
			switch(action = getch()) {
				case '1':
					io_attack(wildPokemon, pcPokemon, 0);
					turn = 0;
					break;

				case '2':
				if(strcmp(pcPokemon->get_move(1),"") != 0){
					io_attack(wildPokemon, pcPokemon, 1);
					turn = 0;
				} else {
					turn = 1;
				}
				break;

				case '3':
				if(strcmp(pcPokemon->get_move(2),"") != 0){
					io_attack(wildPokemon, pcPokemon, 2);
					turn = 0;
				} else {
					turn = 1;
				}
				break;

				case '4':
				if(strcmp(pcPokemon->get_move(3),"") != 0){
					io_attack(wildPokemon, pcPokemon, 3);
					turn = 0;
				} else {
					turn = 1;
				}
				break;

				case 'r':
					turn = io_use_item(0);
					if(turn == 0) {
						int damage;
						if((rand()%100) < wildPokemon->get_move_accuracy(0)) {
							damage = ((((((2*wildPokemon->get_level())/5)+2)*wildPokemon->get_move_power(0) * (wildPokemon->get_atk() / pcPokemon->get_def()))/50)+2);
							pcPokemon->set_health(std::max((pcPokemon->get_health()-damage), 0));
						}		
					}
				break;

				case 'p':
					turn = io_use_item(1);
					if(turn == 0) {
						int damage;
						if((rand()%100) < wildPokemon->get_move_accuracy(0)) {
							damage = ((((((2*wildPokemon->get_level())/5)+2)*wildPokemon->get_move_power(0) * (wildPokemon->get_atk() / pcPokemon->get_def()))/50)+2);
							pcPokemon->set_health(std::max((pcPokemon->get_health()-damage), 0));
						}		
					}
				break;

				case 'b':
					catchResult = io_use_item(2);
					if (catchResult < 0) {
						turn = 1;
					} else if(catchResult == 0) {
						turn = 0;
						delete wildPokemon;
						battleCont = false;
					} else {
						world.pc.buddy[catchResult] = wildPokemon;
						turn = 0;
						battleCont = false;
					}
				break;

				case 's':
					turn = io_swtich(&pcPokemon);
					if(turn == 0) {
						int damage;
						if((rand()%100) < wildPokemon->get_move_accuracy(0)) {
							damage = ((((((2*wildPokemon->get_level())/5)+2)*wildPokemon->get_move_power(0) * (wildPokemon->get_atk() / pcPokemon->get_def()))/50)+2);
							pcPokemon->set_health(std::max((pcPokemon->get_health()-damage), 0));
						}	
					}
				break;

				case 'f':
					if((rand()%100) < 95) {
						battleCont = false;
					} else {
						int damage;
						if((rand()%100) < wildPokemon->get_move_accuracy(0)) {
							damage = ((((((2*wildPokemon->get_level())/5)+2)*wildPokemon->get_move_power(0) * (wildPokemon->get_atk() / pcPokemon->get_def()))/50)+2);
							pcPokemon->set_health(std::max((pcPokemon->get_health()-damage), 0));
						}
					}
					turn = 0;
				break;

				default:
					turn = 1;
					break;
			}
		}while(turn);

		if(wildPokemon->get_health() == 0) {
			delete wildPokemon;
			battleCont = false;
		}

		if(pcPokemon->get_health() == 0) {
			for (int j=0; j<=6; j++) {
				if(j==6 || world.pc.buddy[j] == NULL) {
					world.quit = 2;
					battleCont = false;
					break;
				}

				if(world.pc.buddy[j]->get_health() != 0) {
					pcPokemon = world.pc.buddy[j];
					break;
				}
			}
		}
	}while(battleCont);
}

void io_choose_starter()
{
  class pokemon *choice[3];
  int i;
  bool again = true;
  
  choice[0] = new class pokemon();
  choice[1] = new class pokemon();
  choice[2] = new class pokemon();

  echo();
  curs_set(1);
  do {
    mvprintw( 4, 20, "Before you are three Pokemon, each of");
    mvprintw( 5, 20, "which wants absolutely nothing more");
    mvprintw( 6, 20, "than to be your best buddy forever.");
    mvprintw( 8, 20, "Unfortunately for them, you may only");
    mvprintw( 9, 20, "pick one.  Choose wisely.");
    mvprintw(11, 20, "   1) %s", choice[0]->get_species());
    mvprintw(12, 20, "   2) %s", choice[1]->get_species());
    mvprintw(13, 20, "   3) %s", choice[2]->get_species());
    mvprintw(15, 20, "Enter 1, 2, or 3: ");

    refresh();
    i = getch();

    if (i == '1' || i == '2' || i == '3') {
      world.pc.buddy[0] = choice[(i - '0') - 1];
      delete choice[(i - '0') % 3];
      delete choice[((i - '0') + 1) % 3];
      again = false;
    }
  } while (again);
  noecho();
  curs_set(0);
}


