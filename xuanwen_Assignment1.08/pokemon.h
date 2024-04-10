#ifndef POKEMON_H
#define POKEMON_H

#include <vector>

enum stats {
	hp,
	attack,
	defense,
	special_attack,
	special_defense,
	speed
};

enum gender {
	male,
	female
};

class Pokemon {
	private:
		int index;
		int level;
		std::vector<int> potential_moves;
		int pokemon_move[2];
		int pokemon_stat[6];
		int pokemon_iv[6];
		gender pokemon_gender;
		bool pokemon_shiny;
	
	public:
		Pokemon(int level);
		int get_level() const;
		int get_hp() const;
		int get_attack() const;
		int get_defense() const;
		int get_special_attack() const;
		int get_special_defense() const;
		int get_speed() const;
		const char *get_species() const;
		const char *get_moves(int i) const;
};

#endif