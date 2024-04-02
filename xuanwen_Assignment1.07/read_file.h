#ifndef READ_FILE_H
#define READ_FILE_H

#include <string>

struct pokemon_csv {
	// public:
		int id;
		std::string identifier;
		int species_id;
		int height;
		int weight;
		int base_experience;
		int order;
		int is_default;
};

struct moves_csv {
	// public:
		int id;
		std::string identifier;
		int generation_id;
		int type_id;
		int power;
		int pp;
		int accuracy;
		int priority;
		int target_id;
		int damage_class_id;
		int effect_id;
		int effect_chance;
		int contest_type_id;
		int contest_effect_id;
		int super_contest_effect_id;
};

struct pokemon_moves_csv {
	// public:
		int pokemon_id;
		int version_group_id;
		int move_id;
		int pokemon_move_method_id;
		int level;
		int order;
};

struct pokemon_species_csv {
	// public:
		int id;
		std::string identifier;
		int generation_id;
		int evolves_from_species_id;
		int evolution_chain_id;
		int color_id;
		int shape_id;
		int habitat_id;
		int gender_rate;
		int capture_rate;
		int base_happiness;
		int is_baby;
		int hatch_counter;
		int has_gender_differences;
		int growth_rate_id;
		int forms_switchable;
		int is_legendary;
		int is_mythical;
		int order;
		int conquest_order;
};

struct experience_csv {
	// public:
		int growth_rate_id;
		int level;
		int experience;
};

struct type_names_csv {
	// public:
		int type_id;
		int local_language_id;
		std::string name;
};

struct pokemon_stats_csv {
	// public:
		int pokemon_id;
		int stat_id;
		int base_stat;
		int effort;
};

struct stats_csv {
	// public:
		int id;
		int damage_class_id;
		std::string identifier;
		int is_battle_only;
		int game_index;
};

struct pokemon_types_csv {
	// public:
		 int pokemon_id;
		 int type_id;
		 int slot;
};

// extern pokemon_csv pokemon[1093];
// extern moves_csv moves[845];
// extern pokemon_moves_csv pokemon_moves[528239];
// extern pokemon_species_csv pokemon_species[899];
// extern experience_csv experience[601];
// extern type_names_csv type_names[194];
// extern pokemon_stats_csv pokemon_stats[6553];
// extern stats_csv stats[9];
// extern pokemon_types_csv pokemon_types[1676];

void read_file(std::string filename);

#endif
