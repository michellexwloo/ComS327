#ifndef TERRAIN_H
#define TERRAIN_H

struct single_map{
	int n;
	int s;
	int w;
	int e;
	int map_row;
	int map_col;
	char terrain[21][80];
};

void gen_base(struct single_map *map);
void gen_terrain(struct single_map *map);
void gen_gates(struct single_map *map, int set_n, int set_s, int set_w, int set_e);
void gen_buildings(struct single_map *map);
void print_map(struct single_map *map);

#endif