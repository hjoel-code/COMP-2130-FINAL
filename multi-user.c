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
        int width;
    };

    char alph[] = "ABCDEFGHI"; 
    int index[] = {0,1,2,3,4,5,6,7,8};

    struct AlphIndex alphIndex[NUM_RANGE]; 

    // Mapping of the letters and their associated scores
    for (j=0;j<NUM_RANGE;j++) {
        alphIndex[j].alph = alph[j];
        alphIndex[j].index = index[j];
        alphIndex[j].width = 1;
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
        char NLINE[200];
        char HLINE[200];
        char VLINE[200];
        
        strcpy(NLINE,"");
        strcat(NLINE,"    ");



        strcpy(HLINE,"");
        strcat(HLINE,"  ");

        strcpy(VLINE,"");
        strcat(VLINE,"  ");
        
        for (int n = 0; n <= 8; n++) {
            strcat(HLINE,"+---");
            strcat(VLINE,"|   ");
            if (alphIndex[n].width > 1) {
                for (int w = 0; w <= alphIndex[n].width-2; w++) {
                    strcat(HLINE,"-");
                    strcat(VLINE," ");
                }
            }
            for (int m = 0; m <= alphIndex[n].width-2; m++) {
                strcat(NLINE," ");
            }
            strncat(NLINE,&alphIndex[n].alph,1);
            strcat(NLINE,"    "); 
            strcat(HLINE,"-");
            strcat(VLINE," ");
        }
        strcat(HLINE,"+");
        strcat(VLINE,"|");

        printf("%s\n",NLINE);
        printf("%s\n",HLINE);
        for (j = 0; j < NUM_RANGE; j++)
        {  
            printf("%s\n",VLINE);
            printf("%d ",j+1);
            for (k = 0; k < NUM_RANGE; k++)
            {  
                if(strcmp(grid[k][j],"   ")==0){
                    char space[40];
                    strcpy(space,"   ");
                    for (int m = 0; m <= alphIndex[k].width-2; m++) {
                        strcat(space," ");
                    }
                    printf("| %s",space);
                }else{
                    if (alphIndex[k].width > strlen(grid[k][j])) {
                        int dif = alphIndex[k].width - strlen(grid[k][j]);
                        char input[30];
                        strcpy(input,"");
                        for (int m = 0; m <= dif-1; m++) {
                            strcat(input," ");
                        }
                        strcat(input,grid[k][j]);
                        printf("| %s  ",input);
                    } else {
                        printf("| %s  ",grid[k][j]);
                    }
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
        int len = strlen(c);

        if (alphIndex[x].width < len)
            alphIndex[x].width = len;

        grid[x][y-1]=c;
        return;
    }

    getNewBoard();
    drawBoard();


    makePlay('A', 8, "Joel Henry");
    drawBoard();

    makePlay('D', 1, "Joel");
    drawBoard();

    makePlay('I', 5, "Joel Henry");
    drawBoard();

    makePlay('A', 9, "Joe");
    drawBoard();

    makePlay('A', 1, "Legendary");
    drawBoard();

    return 0;
}


    

