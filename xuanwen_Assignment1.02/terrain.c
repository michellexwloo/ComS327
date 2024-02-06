#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "terrain.h"

void gen_base(struct single_map *map){
    int r, c;

    for(r = 0; r < 21; r++){
        for(c = 0; c < 80; c++){
            // If on the edge of the map, assign boulder terrain type
            if((c % 79) == 0 || (r % 20) == 0){
                map[r][c] = '%';
            }

            // If inside the map, assign clearing terrain type
            else{
                map[r][c] = '.';
            }
        }
    }
}

void fill_terrain_quad(struct single_map *map, int terrain_x, int terrain_y, int quad_x_mult, int quad_y_mult, char terrain_char){
    int r, c;

    // Height: 3 - 6
    int terrain_height = rand() % 4 + 3;
    // Width: 5 - 9
    int terrain_width = rand() % 5 + 5;

    for(r = 0; r < terrain_height; r++){
        // Check if the matrix position is a border
        if(!((terrain_y + (r * quad_y_mult)) <= 0 || (terrain_y + (r * quad_y_mult)) >= 20)){
            for(c = 0; c < terrain_width; c++){
                if(!((terrain_x + (c * quad_x_mult)) <= 0 || (terrain_x + (c * quad_x_mult)) >= 79)){
                    // Adding a multiplier to generate 4 different sized quadrants, avoiding square terrain patches
                    map->terrain[terrain_y + (r * quad_y_mult)][terrain_x + (c * quad_x_mult)] = terrain_char;
                }
            }
        }
    }
}

void gen_terrain(struct single_map *map){
    int r, c;
    int type_of_terrain;

    for(type_of_terrain = 1; type_of_terrain < 8; type_of_terrain++){
        // Generate within 1-78, 0 and 79 are off limits (borders)
        int terrain_x = rand() % 78 + 1;
        // Generate within 1-19, 0 and 20 are off limits (borders)
        int terrain_y = rand() % 19 + 1;
        // Character for the terrain type to be generated
        char terrain_char = '.';

        // Generate a total of 4 regions of tall grass
        if((type_of_terrain % 4 == 1) || (type_of_terrain % 4 == 2)){
            terrain_char = ':';
        }

        // Generate a total of 2 regions of water
        else if(type_of_terrain % 4 == 3){
            terrain_char = '~';
        }

        // Generate a total of 1 region of boulders
        else{
            terrain_char = '%';
        }

        // Fill in Quadrant 1
        fill_terrain_quad(map, terrain_x, terrain_y, -1, -1, terrain_char);
        // Fill in Quadrant 2
        fill_terrain_quad(map, terrain_x, terrain_y, 1, -1, terrain_char);
        // Fill in Quadrant 3
        fill_terrain_quad(map, terrain_x, terrain_y, -1, 1, terrain_char);
        // Fill in Quadrant 4
        fill_terrain_quad(map, terrain_x, terrain_y, 1, 1, terrain_char);
    }

    for(r = 1; r < 20; r++){
        for(c = 1; c < 79; c++){
            int chance_for_tree_or_boulder = rand() % 50;

            // 2% chance for a tree to spawn
            if(chance_for_tree_or_boulder == 0){
                map->terrain[r][c] = '^';
            }

            // 2% chance for a random rock and boulder to spawn
            if(chance_for_tree_or_boulder == 1){
                map->terrain[r][c] = '%';
            }
        }
    }
}

void gen_NS_path(struct single_map *map){
    int r, c, smaller_gate_val, bigger_gate_val;
    // Leave some space between path and border for potential Pokemon Center or Pokemart
    int path_correction = rand() % 15 + 3;

    if(map->n < map->s){
        smaller_gate_val = map->n;
        bigger_gate_val = map->s;
    }else{
        smaller_gate_val = map->s;
        bigger_gate_val = map->n;
    }

    for(r = 0; r < path_correction; r++){
        map->terrain[r][map->n] = '#';
    }

    for(c = smaller_gate_val; c <= bigger_gate_val; c++){
        map->terrain[path_correction][c] = '#';
    }

    for(r = path_correction; r < 21; r++){
        map->terrain[r][map->s] = '#';
    }
}

void gen_WE_path(struct single_map *map){
    int r, c, smaller_gate_val, bigger_gate_val;
    // Leave some space between path and border for potential Pokemon Center or Pokemart
    int path_correction = rand() % 74 + 3;

    if(map->w < map->e){
        smaller_gate_val = map->w;
        bigger_gate_val = map->e;
    }else{
        smaller_gate_val = map->e;
        bigger_gate_val = map->w;
    }

    for(c = 0; c < path_correction; c++){
        map->terrain[map->w][c] = '#';
    }

    for(r = smaller_gate_val; r <= bigger_gate_val; r++){
        map->terrain[r][path_correction] = '#';
    }

    for(c = path_correction; c < 80; c++){
        map->terrain[map->e][c] = '#';
    }
}

void gen_gates(struct single_map *map, int set_n, int set_s, int set_w, int set_e){
    // Can generate within 3-76. I am trying to space out so a Pokemon Center and Pokemart could fit
    if(set_n < 0){
        map->n = rand() % 74 + 3;
    }
    else{
        map->n = set_n;
    }

    if(set_s < 0){
        map->s = rand() % 74 + 3;
    }
    else{
        map->s = set_s;
    }

    gen_NS_path(map);

    if(map->map_row == -200){
        map->terrain[0][map->n] = '%';
    }

    if(map->map_row == 200){
        map->terrain[20][map->s] = '%';
    }

    // Can generate within 3-17. I am trying to space out so a Pokemon Center and Pokemart could fit
    if(set_w < 0){
        map->w = rand() % 15 + 3;
    }
    else{
        map->w = set_w;
    }

    if(set_e < 0){
        map->e = rand() % 15 + 3;
    }
    else{
        map->e = set_e;
    }

    gen_WE_path(map);

	if(map->map_col == -200){
        map->terrain[map->w][0] = '%';
    }

    if(map->map_col == 200){
        map->terrain[map->e][79] = '%';
    }
}

bool vert_buildings(struct single_map *map, int building_spawn_x, int building_spawn_y){
    int r;
    bool building = true;

    for(r = 1; r <= 2; r++){
        // If 2 spots top or bottom is a road, can't spawn a building
        if((map->terrain[building_spawn_y - r][building_spawn_x] == '#') || (map->terrain[building_spawn_y + r][building_spawn_x] == '#')){
            building = false;
            break;
        }

        // If 2 spots to the top or bottom of the square to the right is a road, can't spawn a building
        if((map->terrain[building_spawn_y - r][building_spawn_x + 1] == '#') || (map->terrain[building_spawn_y + r][building_spawn_x + 1] == '#')){
            building = false;
            break;
        }
    }

    return building;
}

bool hor_buildings(struct single_map *map, int building_spawn_x, int building_spawn_y){
    int c;
    bool building = true;

    for(c = 1; c <= 2; c++){
        // If 2 spots to the left or right is a road, can't spawn a building
        if((map->terrain[building_spawn_y][building_spawn_x - c] == '#') || (map->terrain[building_spawn_y][building_spawn_x + c] == '#')){
            building = false;
            break;
        }

        // If 2 spots to the left or right of the square below is a road, can't spawn a building
        if((map->terrain[building_spawn_y + 1][building_spawn_x - c] == '#') || (map->terrain[building_spawn_y + 1][building_spawn_x + c] == '#')){
            building = false;
            break;
        }
    }

    return building;
}

void gen_buildings(struct single_map *map){
    int r, c;
    // Can generate within 3-76
    int building_spawn_x = rand() % 74 + 3;
    // Can generate within 3-17
    int building_spawn_y = rand() % 15 + 3;

    // Reselect if the randomly selected piece isn't a path
    if(map->terrain[building_spawn_y][building_spawn_x] != '#'){
        gen_buildings(map);
    } else if(vert_buildings(map, building_spawn_x, building_spawn_y) == true){
        for(r = 1; r <= 2; r++){
            for(c = 0; c <= 1; c++){
                map->terrain[building_spawn_y + r][building_spawn_x + c] = 'M';
                map->terrain[building_spawn_y - r][building_spawn_x + c] = 'C';
            }
        }
    } else if(hor_buildings(map, building_spawn_x, building_spawn_y) == true){
        for(r = 0; r <= 1; r++){
            for(c = 1; c <= 2; c++){
                map->terrain[building_spawn_y + r][building_spawn_x + c] = 'M';
                map->terrain[building_spawn_y + r][building_spawn_x - c] = 'C';
            }
        }
    } else{
        gen_buildings(map);
    }
}

void print_map(struct single_map *map){
    int r, c;

    for(r = 0; r < 21; r++){
        for(c = 0; c < 80; c++){
            printf("%c", map->terrain[r][c]);
        }
        printf("\n");
    }
	printf("Map coordinates: (%d, %d) ", map->map_col, map->map_row);
}
