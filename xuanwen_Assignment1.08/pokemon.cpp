#include <unistd.h>
#include <cstdlib>
#include <climits>
#include <cctype>
#include <ncurses.h>
#include <math.h>

#include "pokemon.h"
#include "db_parse.h"

Pokemon::Pokemon(int level) : level(level) {
	int i, move1, move2;
	index = (rand() % 898) + 1;

	do {
		move1 = (rand() % 528238) + 1;
		pokemon_move[0] = pokemon_moves[move1].move_id;
	}while(!((pokemon_moves[move1].pokemon_id == pokemon[index].species_id) &&
			(pokemon_moves[move1].pokemon_move_method_id == 1)));
	
	do {
		move2 = (rand() % 528238) + 1;
		pokemon_move[1] = pokemon_moves[move2].move_id;
	}while(!((pokemon_moves[move2].pokemon_id == pokemon[index].species_id) &&
			(pokemon_moves[move2].pokemon_move_method_id == 1)) || (move1 == move2));

	for(i = 0; i < 6; i++) {
		pokemon_iv[i] = rand() % 16;
		if(i == 0) {
			pokemon_stat[i] = (((((pokemon_stats[1 + (index-1) * 6].base_stat + pokemon_iv[i]) * 2) * level) / 100) + level + 10);
		}else {
			pokemon_stat[i] = (((((pokemon_stats[1 + (index-1) * 6].base_stat + pokemon_iv[i]) * 2) * level) / 100) + 5);		
		}
	}

	if(rand() % 2 == 0) {
		pokemon_gender = male;
	}else {
		pokemon_gender = female;
	}

	if(rand() % 8192 == 0) {
		pokemon_shiny = true;
	}else {
		pokemon_shiny = false;
	}
}

int Pokemon::get_level() const {
	return level;
}

int Pokemon::get_hp() const {
	return pokemon_stat[hp];
}

int Pokemon::get_attack() const {
	return pokemon_stat[attack];
}

int Pokemon::get_defense() const {
	return pokemon_stat[defense];
}

int Pokemon::get_special_attack() const {
	return pokemon_stat[special_attack];
}

int Pokemon::get_special_defense() const {
	return pokemon_stat[special_defense];
}

int Pokemon::get_speed() const {
	return pokemon_stat[speed];
}

const char *Pokemon::get_species() const {
	return species[index].identifier;
}

const char *Pokemon::get_moves(int i) const {
	return moves[pokemon_move[i]].identifier;
}