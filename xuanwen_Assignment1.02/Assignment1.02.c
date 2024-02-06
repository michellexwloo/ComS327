#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include "terrain.h"

void gen_map(struct single_map *world[401][401], int world_r, int world_c) {

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

	}while(input != 'q');

	return 0;
}