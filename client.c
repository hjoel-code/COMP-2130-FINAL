#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>


#define BUF_SIZE	1024
#define	SERVER_IP	"127.0.0.1"
#define SERVER_PORT	60000
#define NUM_RANGE 9

int	sock_send;
struct sockaddr_in	addr_send;
int	i;
char text[80],buf[BUF_SIZE];
int	send_len,bytes_sent,bytes_received;

struct UserInput {
    char coord[2];
    int user;
    char inp[10];
};

int uid;
struct UserInput user_inpts[82];
int input_count = 0;

char cell[3];
char cellVal[20];
char inpType;


// ++++++++ GRID FUNCTIONS ++++++++++++++++++++++++++++++++++

int k,j;

//global declaration structure grid
char * grid[NUM_RANGE][NUM_RANGE];

struct AlphIndex {
    // Dictionary Structure to store the letter and score associated with a letter
    char alph;
    int index;
    int width;
};

char alph[] = "ABCDEFGHI"; 
int idx[] = {0,1,2,3,4,5,6,7,8};

struct AlphIndex alphIndex[NUM_RANGE]; 


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

int isOnBoard(char alph, int y) {
        int x = searchIndex(alph);

        if ((x <= NUM_RANGE && x > 0) && (y <= NUM_RANGE && y > 0)) {
            return 1;
        }

        return -1;
    }
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void updateClient(char *init) {
        char *tok_end;
        char *token = strtok_r(init, ":", &tok_end);

        while (token != NULL) {

            if (strlen(token) <= 3) {
                printf("%s\n",token);
                uid = atoi(token);
            } else {
                char *tok1_end;
                char *token1 = strtok_r(token, ",", &tok1_end);
                int var = 0;
                while (token1 != NULL) {
                    if (var == 0) {
                        strcpy(user_inpts[input_count].coord,token1);
                    } else if (var == 1) {
                        user_inpts[input_count].user = atoi(token1);
                    } else {
                        strcpy(user_inpts[input_count].inp,token1);
                    }
                    token1 = strtok_r(NULL, ",", &tok1_end);
                    var++;
                }
                makePlay(user_inpts[input_count].coord[0], user_inpts[input_count].coord[1]-'0', user_inpts[input_count].inp);
                input_count++;
            }
            token = strtok_r(NULL, ":", &tok_end);
        }
    }

void *server_updates(void *arg) {
        getNewBoard();
        updateClient((char *)(intptr_t)arg);
        drawBoard();
        while(1) {
            printf("\nPress any key and ENTER to continue\n");
            bytes_received = recv(sock_send,buf,BUF_SIZE,0);
            buf[bytes_received] = 0;
            printf("\nUpdate Received %d: %s \n",sock_send,buf);
            char *tok;
            char *token = strtok_r(buf, ",", &tok);
            int var = 0;
            while (token != NULL) {
                if (var == 0) {
                    strcpy(user_inpts[input_count].coord,token);
                } else if (var == 1) {
                    user_inpts[input_count].user = atoi(token);
                } else {
                    strcpy(user_inpts[input_count].inp,token);
                }
                token = strtok_r(NULL, ",", &tok);
                var++;
            }

            makePlay(user_inpts[input_count].coord[0], user_inpts[input_count].coord[1]-'0', user_inpts[input_count].inp);
            input_count++;
            drawBoard();
        }

        close(sock_send);
    }

void getInputType(char *res, char *val) {
    int d = atoi(val);
    float f = atof(val);

    if (val[0] == '=') {
        strcat(res, "F");
        return;
    } else if (d != 0) {
        strcat(res, "N");
        return;
    } else if (f != 0) {
        strcat(res, "N");
        return;
    } else {
        strcat(res, "T");
        return;
    }
}

void getFunctionType(char *res, char *val) {
    char f_type[20];
    int len = strlen(val);
    strcpy(f_type, "");
    for (int n = 1; n < len; n++) {
        if (val[n] == '(')
            break;
        strncat(f_type, &val[n], 1);
    }

    if (strcmp(f_type, "AVERAGE") == 0) {
        strcat(res,"A");
        return;
    } else if (strcmp(f_type, "average") == 0) {
        strcat(res,"A");
        return;
    } else if (strcmp(f_type, "SUM") == 0) {
        strcat(res,"S");
        return;
    } else if (strcmp(f_type, "sum") == 0) {
        strcat(res,"S");
        return;
    } else if (strcmp(f_type, "range") == 0) {
        strcat(res,"R");
        return;
    } else if (strcmp(f_type, "RANGE") == 0) {
        strcat(res,"R");
        return;
    } else {
        strcpy(res,"INVALID-FUNCTION");
        return;
    }
}

void getFunctionRange(char *res, char *val) {
    char ranges[20];
    char lim;
    int len = strlen(val);
    strcpy(ranges, "");

    
    for (int n = 1; n < len; n++) {
        lim = val[n];
        if (lim == '(') {
            while (lim != ')') {
                n++;
                if (val[n] == ')') {
                    lim = val[n];
                } else {
                    strncat(ranges, &val[n], 1);
                }
            }
        } else if (lim == ')') {
            break;
        }
    }

    int chk = 0;
    char *cell;
    cell = strtok(ranges,",");


    while (cell != NULL) {
        if (cell[0] >= 'a' && cell[0] <= 'z')
            cell[0] = cell[0] - 32;
        
        if (isOnBoard(cell[0], cell[1]-'0') == 1) {
            strcat(res,cell);
            if (chk == 0)
                strcat(res,",");
            cell = strtok(NULL,",");
        } else {
            strcpy(res,"INVALID-RANGE-CELL");
            cell = NULL;
        }
        chk++;   
    }
}

int main(int argc, char *argv[]) {  
    // Mapping of the letters and their associated scores
    for (j=0;j<NUM_RANGE;j++) {
        alphIndex[j].alph = alph[j];
        alphIndex[j].index = idx[j];
        alphIndex[j].width = 1;
    };

    /* create socket for sending data */
    sock_send=socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock_send < 0){
        printf("socket() failed\n");
        exit(0);
    }

    /* create socket address structure to connect to */
    memset(&addr_send, 0, sizeof (addr_send)); /* zero out structure */
    addr_send.sin_family = AF_INET; /* address family */
    addr_send.sin_addr.s_addr = inet_addr(SERVER_IP);
    addr_send.sin_port = htons((unsigned short)SERVER_PORT);

    /* connect to the server */
    i=connect(sock_send, (struct sockaddr *) &addr_send, sizeof (addr_send));
    if (i < 0){
        printf("connect() failed\n");
        exit(0);
    }

    bytes_received = recv(sock_send,buf,BUF_SIZE,0);
    buf[bytes_received] = 0;
    printf("Received %d: %s \n",sock_send,buf);

    
    pthread_t thread_id;
    int thread = pthread_create(&thread_id, NULL, server_updates, (void *)(intptr_t)buf);
    if (thread != 0) {
        printf("Failed to connect to server for updates\n");
    }
    

    void validateCellIndex(char *cell) {
        while (1) {
            if (cell[0] >= 'a' && cell[0] <= 'z')
                cell[0] = cell[0] - 32;
            
            if (isOnBoard(cell[0], cell[1]-'0') ==  1)
                break;
            printf("Cell Index: ");
            scanf("%s",cell);
        }
        return;
    }

    while (1){
        /* send some data */
        printf("Cell Index: ");
        scanf("%s",cell);
        validateCellIndex(cell);

        printf("Value: ");
        scanf("%s",cellVal);

        sprintf(text,"%d",uid);
        strcat(text,":");

        getInputType(text, cellVal);

        inpType = text[strlen(text)-1];
        strcat(text,":");
        
        strcat(text,cell);
        strcat(text,":");
        

        if (inpType == 'F') {
            getFunctionType(text,cellVal);
            if (strcmp(text, "INVALID-FUNCTION") == 0) {
                printf("-- %s\n", text);
            } else {
                strcat(text,":");
                getFunctionRange(text, cellVal);
                if (strcmp(text, "INVALID-RANGE-CELL") == 0) {
                    printf("-- %s\n", text);
                } else {
                    strcpy(buf,text);
                    send_len=strlen(text);
                    printf("\n%s\n",buf);
                    bytes_sent=send(sock_send,buf,send_len,0);
                }
            }
        } else {
            strcat(text,cellVal);
            strcpy(buf,text);
            send_len=strlen(text);
            printf("\n%s\n",buf);
            bytes_sent=send(sock_send,buf,send_len,0);
        }
        
        if (strcmp(text,"quit") == 0)
            break;
    }
    
    pthread_join(thread_id, NULL);
    close(sock_send);
    
}


// uid,type(FUNC,INT,TEXT),CELL1,CELL2(A2,C2)

// uid:type:C3:A2B1


