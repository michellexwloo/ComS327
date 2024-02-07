#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include "terrain.h"

void gen_map(struct single_map *world[401][401], int world_r, int world_c) {
	world[world_r][world_c] = malloc(sizeof(struct single_map));
	world[world_r][world_c]->map_row = world_r - 200;
	world[world_r][world_c]->map_col = world_c - 200;

	gen_base(world[world_r][world_c]);
	gen_terrain(world[world_r][world_c]);

	int set_n = -1;
	int set_s = -1;
	int set_w = -1;
	int set_e = -1;

// Connecting gates of neighboring maps
	// Not at top of the world
	if(world_r > 0){
		// An exisiting south gate will be the new map's north gate
		if(world[world_r-1][world_c] != NULL){
			set_n = world[world_r-1][world_c]->s;
		}
	}
	// Not at bottom of the world
	if(world_r < 400){
		// An exisiting north gate will be the new map's south gate
		if(world[world_r+1][world_c] != NULL){
			set_s = world[world_r+1][world_c]->n;
		}
	}
	// Not at right most of the world
	if(world_c > 0){
		if(world[world_r][world_c-1] != NULL){
			set_w = world[world_r][world_c-1]->e;
		}
	}
	// Not at right most of the world
	if(world_c < 400){
		if(world[world_r][world_c+1] != NULL){
			set_e = world[world_r][world_c+1]->w;
		}
	}

	gen_gates(world[world_r][world_c], set_n, set_s, set_w, set_e);

// New probability for generating buildings
	// Generate buildings at starting map
	if(world_r==200 && world_c==200){
		gen_buildings(world[world_r][world_c]);
	}else{
		// New probability
		int manhattan_dist = abs(world[world_r][world_c]->map_row) + abs(world[world_r][world_c]->map_col);
		if((rand()%100) < ((-45 * manhattan_dist)/200+50)){
			gen_buildings(world[world_r][world_c]);
		}
	}
}

int main(int argc, char *argv[]) {
	srand(time(NULL));
	int r, c;
	struct single_map *world[401][401];

	// Initialize staring coordinates & maps
	int world_r = 200;
	int world_c = 200;
	int fly_r = 0;
	int fly_c = 0;
	for(r=0; r<401; r++){
		for(c=0; c<401; c++){
			world[r][c] = NULL;
		}
	}

	gen_map(world, world_r, world_c);

	char input = 'z';
	do{
		print_map(world[world_r][world_c]);
		printf("\nEnter command: ");
		scanf(" %c", &input);
		printf("\n");

		// Move immediately north of current map
		if(input == 'n'){
			if(world_c > 0){
				world_c -= 1;
				if(world[world_r][world_c] == NULL){
					gen_map(world, world_r,world_c);
				}
			}
		}

		// Move immediately south of current map
		else if(input == 's'){
			if(world_c < 400){
				world_c += 1;
				if(world[world_r][world_c] == NULL){
					gen_map(world, world_r,world_c);
				}
			}
		}

		// Move immediately east of current map
		else if(input == 'e'){
			if(world_r < 400){
				world_r += 1;
				if(world[world_r][world_c] == NULL){
					gen_map(world, world_r,world_c);
				}
			}
		}

		// Move immediately west of current map
		else if(input == 'w'){
			if(world_r > 0){
				world_r -= 1;
				if(world[world_r][world_c] == NULL){
					gen_map(world, world_r,world_c);
				}
			}
		}

		// Fly to map
		else if(input == 'f'){
			scanf(" %d %d", &fly_r, &fly_c);

			if(fly_r > 200 || fly_c > 200 || fly_r < -200 || fly_c < -200){
				printf("Error: Trying to go out of the world.\n\n");

			}else{
				world_r = fly_r + 200;
				world_c = fly_c + 200;
				if(world[world_r][world_c] == NULL){
					gen_map(world, world_r, world_c);
				}
			}
		}else if(input != 'q'){
			printf("Error: Command '%c' not found.\n\n", input);
		}
		
	}while(input != 'q');

	for(r=0; r<401; r++){
		for(c=0; c<401; c++){
			if(world[r][c] != NULL){
				free(world[r][c]);
			}
		}
	}

	return 0;
}