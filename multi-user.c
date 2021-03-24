#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
 



int main() {
    int k,j;
    int const NUM_RANGE=9;

    //global declaration structure grid
    char * grid[NUM_RANGE][NUM_RANGE];

    struct AlphIndex {
        // Dictionary Structure to store the letter and score associated with a letter
        char alph;
        int index;
    };

    char alph[] = "ABCDEFGHI"; 
    int index[] = {0,1,2,3,4,5,6,7,8};

    struct AlphIndex alphIndex[NUM_RANGE]; 

    // Mapping of the letters and their associated scores
    for (j=0;j<NUM_RANGE;j++) {
        alphIndex[j].alph = alph[j];
        alphIndex[j].index = index[j];
    };

    // Method to search through an array of structures to identify which score matches the letter
    int searchIndex(char letter) {
        for (int n=0;n<NUM_RANGE;n++) {
            if (alphIndex[n].alph == letter) {
                return alphIndex[n].index;
            }
        }
        return 0;
    }

    void drawBoard(){
        //This function prints out the board that was passed. Returns void
        char * const NLINE = "    A    B    C    D    E    F    G    H    I";
        char * const HLINE = "  +----+----+----+----+----+----+----+----+----+";
        char * const VLINE = "  |    |    |    |    |    |    |    |    |    |";

        printf("%s\n",NLINE);
        printf("%s\n",HLINE);
        for (j = 0; j < NUM_RANGE; j++)
        {  
            printf("%s\n",VLINE);
            printf("%d ",j+1);
            for (k = 0; k < NUM_RANGE; k++)
            {  if(strcmp(grid[k][j],"   ")==0){
                    printf("| %s",grid[k][j]);
                }else{
                    printf("| %s  ",grid[k][j]);
                }
            }
            printf("%s","|");
            printf("\n");
            printf("%s\n",VLINE);
            printf("%s\n",HLINE);
        }
        return;
    }//EndFunction drawBoard

    void getNewBoard(){
            //creates a brand new blank board. Returns a pointer to the array 
            for (j = 0; j < NUM_RANGE; j++){
                for (k = 0; k < NUM_RANGE; k++)
                {
                    grid[k][j]="   ";
                }//Endfor
            }//Endfor
            return;
        }//EndFunction getNewBoard
    
    void makePlay(char alph, int y, char* c){
        int x = searchIndex(alph);

        grid[x][y-1]=c;
        return;
    }

    getNewBoard();
    drawBoard();


    makePlay('A', 8, "H");
    drawBoard();

    return 0;
}


    

