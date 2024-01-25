#include <stdio.h>
#include <stdbool.h>

#define SIZE 5

bool worked[SIZE][SIZE][8],visited[SIZE][SIZE];
int row_move[8] = {-2,-2,-1,-1,1,1,2,2};
int col_move[8] = {-1,1,-2,2,-2,2,-1,1};
int p,total;

int result[1728][25];

void tour (int r, int c){
    if (p >= 25){
        for (int i = 0; i < 25; i++){
            result[total+1][i] = result[total][i];
        }
        total++;
        return;
    }
    
    int i,rt, ct;
    visited[r][c] = true;

    for(i=0; i<8; i++){
        rt = r + row_move[i];
        ct = c + col_move[i];

        if(rt<0 || rt>SIZE-1 || ct<0 || ct>SIZE-1 || visited[rt][ct]) continue;

        result[total][p++] = rt * SIZE + ct + 1;
        tour(rt,ct);
        p--;
    }
    visited[r][c] = false;
}

int main (int argc, char * argv[]){
    int i,j;
    total = 0;

    for(i=0; i<SIZE; i++){
        for(j=0; j<SIZE; j++){
            p = 1;
            result[total][0] = i * SIZE + j + 1;
            tour(i,j);
        }
    }

    for(i = 0; i < 1728; i++){
        printf("%d", result[i][0]);
        for(j = 1; j < 25; j++){
            printf(",%d", result[i][j]);
        }
        printf("\n");
    }
    return 0;
}