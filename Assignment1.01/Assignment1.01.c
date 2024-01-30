# include <stdio.h>
# include <stdlib.h>
#include <time.h>
#include <stdbool.h>

void generate_map(char map[21][80]){
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

void fill_terrain_quadrant(char map[21][80], int terrain_x, int terrain_y, int quad_x_mult, int quad_y_mult, char terrain_char){
    int r, c;

    // Minimum height of 3 and max height of 6
    int terrain_height = rand() % 4 + 3;
    // Minimum width of 5 and max width of 9
    int terrain_width = rand() % 5 + 5;

    // Essentially the matrix generation that we all love and know with a couple extra things...
    for(r = 0; r < terrain_height; r++){
        // Check if the matrix position is a border. If so, don't change it!
        if(!((terrain_y + (r * quad_y_mult)) <= 0 || (terrain_y + (r * quad_y_mult)) >= 20)){
            for(c = 0; c < terrain_width; c++){
                if(!((terrain_x + (c * quad_x_mult)) <= 0 || (terrain_x + (c * quad_x_mult)) >= 79)){
                    // Adding a multiplier so we can essentially generate 4 different sized quadrants to avoid square terrain patches
                    map[terrain_y + (r * quad_y_mult)][terrain_x + (c * quad_x_mult)] = terrain_char;
                }
            }
        }
    }
}

void generate_terrain(char map[21][80]){
    int r, c;
    // Counter for the type of terrain
    int type_of_terrain;

    for(type_of_terrain = 1; type_of_terrain < 8; type_of_terrain++){
        // Can generate within 1-78, 0 and 79 are off limits (borders)
        int terrain_x = rand() % 78 + 1;
        // Can generate within 1-19, 0 and 20 are off limits (borders)
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

        // The idea here is to have a middle point and treat it as the 4 different corners of a square, 
        // but each of the squares are different sizes, so it avoids making a square (most likely)

        // Fill in Quadrant 1
        fill_terrain_quadrant(map, terrain_x, terrain_y, -1, -1, terrain_char);
        // Fill in Quadrant 2
        fill_terrain_quadrant(map, terrain_x, terrain_y, 1, -1, terrain_char);
        // Fill in Quadrant 3
        fill_terrain_quadrant(map, terrain_x, terrain_y, -1, 1, terrain_char);
        // Fill in Quadrant 4
        fill_terrain_quadrant(map, terrain_x, terrain_y, 1, 1, terrain_char);
    }

    for(r = 1; r < 20; r++){
        for(c = 1; c < 79; c++){
            int chance_for_tree_or_boulder = rand() % 50;

            // 2% chance for a tree to spawn in a square
            if(chance_for_tree_or_boulder == 0){
                map[r][c] = '^';
            }

            // 2% chance for a RANDOM boulder to spawn in a square
            if(chance_for_tree_or_boulder == 1){
                map[r][c] = '%';
            }
        }
    }
}

void print_map(char map[21][80]){
    int r, c;

    for(r = 0; r < 21; r++){
        for(c = 0; c < 80; c++){
            printf("%c", map[r][c]);
        }
        printf("\n");
    }
}

int main(int argc, char *argv[]){
    char map[21][80];
    srand(time(NULL));

    generate_map(map);
    generate_terrain(map);
    print_map(map);
    return 0;
}