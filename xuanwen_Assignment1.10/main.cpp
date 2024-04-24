#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iomanip>

using namespace std;

#define COLOR_RED	"\x1b[31m"
#define COLOR_YELLOW	"\x1b[33m"
#define COLOR_GREEN 	"\x1b[32m"
#define COLOR_BLUE	"\x1b[34m"
#define COLOR_MAGENTA 	"\x1b[35m"
#define COLOR_ORANGE	"\x1b[38;5;202m"
#define COLOR_CYAN 	"\x1b[36m"
#define COLOR_OLIVE	"\x1b[38;5;58m"
#define COLOR_PINK	"\x1b[38;5;201m"
#define COLOR_AQUA	"\x1b[38;2;145;231;255m"
#define COLOR_LAVENDER	"\x1b[38;5;147m"
#define COLOR_GRAY	"\x1b[38;5;242m"
#define COLOR_RESET   "\x1b[0m"

void move_up(int a[4][4]) {
    int i,j,r,c;
    for(j=0;j<4;j++) {
        r=0,c=j;
        for(i=1;i<4;i++) {
            if(a[i][j]!=0) {
                if(a[i-1][j]==0 || a[i-1][j]==a[i][j]) {
                    if(a[r][c]==a[i][j]) {
                        a[r][c]*=2;
                        a[i][j]=0;
                    } else {
                        if(a[r][c]==0) {
                            a[r][c]=a[i][j];
                            a[i][j]=0;
                        } else {
                            a[++r][c]=a[i][j];
                            a[i][j]=0;
                        }
                    }
                }
                else r++;
            }
        }
    }
}

void move_down(int a[4][4]) {
    int i,j,r,c;
    for(j=0;j<4;j++) {
        r=3,c=j;
        for(i=2;i>=0;i--) {
            if(a[i][j]!=0) {
                if(a[i+1][j]==0 || a[i+1][j]==a[i][j]) {
                    if(a[r][c]==a[i][j]) {
                        a[r][c]*=2;
                        a[i][j]=0;
                    } else {
                        if(a[r][c]==0) {
                            a[r][c]=a[i][j];
                            a[i][j]=0;
                        } else {
                            a[--r][c]=a[i][j];
                            a[i][j]=0;
                        }
                    }
                }
                else r--;
            }
        }
    }
}

void move_left(int a[4][4]) {
    int i,j,r,c;
    for(i=0;i<4;i++) {
        r=i,c=0;
        for(j=1;j<4;j++) {
            if(a[i][j]!=0) {
                if(a[i][j-1]==0 || a[i][j-1]==a[i][j]) {
                    if(a[r][c]==a[i][j]) {
                        a[r][c]*=2;
                        a[i][j]=0;
                    } else {
                        if(a[r][c]==0) {
                            a[r][c]=a[i][j];
                            a[i][j]=0;
                        } else {
                            a[r][++c]=a[i][j];
                            a[i][j]=0;
                        }
                    }
                }
                else c++;
            }
        }
    }
}

void move_right(int a[4][4]) {
    int i,j,r,c;
    for(i=0;i<4;i++) {
        r=i,c=3;
        for(j=2;j>=0;j--) {
            if(a[i][j]!=0) {
                if(a[i][j+1]==0 || a[i][j+1]==a[i][j]) {
                    if(a[r][c]==a[i][j]) {
                        a[r][c]*=2;
                        a[i][j]=0;
                    } else {
                        if(a[r][c]==0) {
                            a[r][c]=a[i][j];
                            a[i][j]=0;
                        } else {
                            a[r][--c]=a[i][j];
                            a[i][j]=0;
                        }
                    }
                }
                else c--;
            }
        }
    }
}

// Check if the cell is empty
int cell_occupied(int tmp[4][4],int a[4][4]) {
    int cell=1,i,j;
    for(i=0; i<4; i++) {
        for(j=0; j<4; j++) {
            if(tmp[i][j]!=a[i][j]) {
                cell = 0;
                break;
            }
		}
	}
    return cell;
}

void new_block(int a[4][4]) {
    int r,c;
    srand(time(0));
    while(1) {
        r=rand()%4;
        c=rand()%4;
        if(a[r][c]==0) {
            a[r][c]=pow(2,r%2 + 1);
            break;
        }
    }
}

void display(int a[4][4]) {
    cout<<"\n\t\tPress q anytime to quit the game";
	cout<<"\n\t\tUse w a s d for control";
    cout<<"\n\n\n";
    int i,j;
	int borderLength = 6 * 4 + 1;
    for(i=0;i<4;i++) {
		cout << "\t\t";
        for (int k = 0; k < borderLength; k++) {
            cout << "-";
        }
        cout << endl << "\t\t";
        for(j=0;j<4;j++) {
			cout << "|";
            if(a[i][j]==0) {
				cout<<setw(5) <<" ";
			} else {
				if(a[i][j] == 2) {
					cout << COLOR_RED;
				} else if(a[i][j] == 4) {
					cout << COLOR_GREEN;
				} else if(a[i][j] == 8) {
					cout << COLOR_YELLOW;
				} else if(a[i][j] == 16) {
					cout << COLOR_BLUE;
				} else if(a[i][j] == 32) {
					cout << COLOR_MAGENTA;
				} else if(a[i][j] == 64) {
					cout << COLOR_CYAN;
				} else if (a[i][j] == 128) {
					cout << COLOR_ORANGE;
				} else if (a[i][j] == 256) {
					cout << COLOR_OLIVE;
				} else if (a[i][j] == 512) {
					cout << COLOR_PINK;
				} else if (a[i][j] == 1024) {
					cout << COLOR_AQUA;
				} else if (a[i][j] == 2048) {
					cout << COLOR_LAVENDER;
				} else {
					cout << COLOR_GRAY;
				}
				cout << setw(4) << a[i][j] << " " << COLOR_RESET;
			}
        }
        cout<<"|"<<endl;
    }
	cout << "\t\t";
        for (int k = 0; k < borderLength; k++) {
            cout << "-";
        }
        cout << endl << "\t\t";
}

// Check if there are any more possible moves.
// Return 0 when there is none, 1 otherwise
int check_over(int a[4][4]) {
    int fl=0,gl=0,i,j;
    for(i=0; i<4; i++)
        for(j=0; j<4; j++)
            if(a[i][j]==0) {
                fl=1;
                break;    
            }
            
    for(i=0; i<3; i++)
        for(j=0; j<3; j++)
            if(a[i+1][j]==a[i][j] || a[i][j+1]==a[i][j]) {
                gl=1;
                break;
            }
            
    if(fl || gl) { 
		return 1;
	} else { 
		return 0;
	}
}

// Check if 2048
int check_win(int a[4][4]) {
	int i,j;
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
			if(a[i][j] == 2048) {
				return 1;
			}
	
	return 0;
}

int main() {
    cout<<"\n\n\n\n\t\t\t2048 GAME\n\n\n\t\tPress enter to start";
    getchar();
    system("clear");
    int i1,i2,i3,i4,i,j;
    int a[4][4]={0},tmp[4][4]={0};
    srand(time(0));
    i1=rand()%4;
    i2=rand()%4;
    while(1) {
        i3=rand()%4;
        i4=rand()%4;
        if(i3!=i1 && i4!=i2) break;
    }
    a[i1][i2]=2;
    a[i3][i4]=4;
    display(a);
    
    int ch;
	bool firstWin = true;
    while (1) {
        for(i=0;i<4;i++)
            for(j=0;j<4;j++)
                tmp[i][j]=a[i][j];
        ch=getchar();
        system("clear");
        if(ch=='w') {move_up(a);}
        else if(ch=='s') {move_down(a);}
        else if(ch=='a') {move_left(a);}
        else if(ch=='d') {move_right(a);}
        else if(ch=='q') {
			cout << "Goodbye" <<endl;
			break;
		}

		// check 2048 (only for first time)
		if(firstWin && check_win(a)) {
			cout<<"\n\n\t\t\t2048! You won!!!\n";
			cout<<"\t\tDo you want to continue playing? (y/n): ";
			char cont;
			cin >> cont;
			system("clear");
			if(cont == 'y' || cont == 'Y') {
				firstWin = false;
			}else{
				cout << "Goodbye 2048" <<endl;
				return 0;
			}
		}

		// spawn new block
        if(!cell_occupied(tmp,a)) {
            new_block(a);
		}
        display(a);
            
		// Game Over
        if(!check_over(a)) {
            cout<<"\n\n\t\t\tGAME OVER!!\n\n\n";
            getchar();
            break;
        }    
    }
    return 0;
}