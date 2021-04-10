// 620127501 -- Joel Henry
// 620130814 -- Annwar Greenland
// 620128524 -- Keneil Thompson



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
#define SERVER_PORT	53100
#define NUM_RANGE 9

int	sock_send;
struct sockaddr_in	addr_send;
int	i;
char text[80],buf[BUF_SIZE];
int	send_len,bytes_sent,bytes_received;

// Declaration of spreadsheet input structure
struct UserInput {
    char coord[2];
    int user;
    char inp[10];
};
// Initialization of input structure and input count
struct UserInput user_inpts[82];
int input_count = 0;

// Declaration of user count and uid of the client
int user_count, uid = -1;



char user_input[5];
char cellVal[20];
char inpType;

pthread_t thread_id;


// ++++++++ GRID FUNCTIONS ++++++++++++++++++++++++++++++++++

int k,j;

//global declaration structure grid
char * grid[NUM_RANGE][NUM_RANGE];

// Declaration of grid's columns structure
struct AlphIndex {
    // Stores grid letter to corresponding index
    char alph;
    int index;
    int width;  //Stores width of each column on the grid
};

char alph[] = "ABCDEFGHI"; 
int idx[] = {0,1,2,3,4,5,6,7,8};

struct AlphIndex alphIndex[NUM_RANGE]; 


// Accepts character and returns corresponding index integer
int searchIndex(char letter) {
    for (int n=0;n<NUM_RANGE;n++) {
        if (alphIndex[n].alph == letter) {
            return alphIndex[n].index;
        }
    }
    return 0;
}

void drawWorksheet(){
        //This function prints out the worksheet that was passed. Returns void
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
    }

void getNewSpreadsheet(){
            //creates a brand new worksheet. Returns a pointer to the array 
            for (j = 0; j < NUM_RANGE; j++){
                for (k = 0; k < NUM_RANGE; k++)
                {
                    grid[k][j]="   ";
                }//Endfor
            }//Endfor
            return;
        }//EndFunction getNewSpreadsheet
    
void acceptData(char alph, int y, char* c){
        int x = searchIndex(alph);
        int len = strlen(c);

        if (alphIndex[x].width < len)
            alphIndex[x].width = len;

        grid[x][y-1]=c;
        return;
    }

int isOnWorksheet(char alph, int y) {
        int x = searchIndex(alph);

        if ((x < NUM_RANGE && x >= 0) && (y <= NUM_RANGE && y >= 0)) {
            return 1;
        }

        return -1;
    }
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Receives initial data from server to update worksheet
void updateClient(char *init) {
        char *tok_end;
        char *token = strtok_r(init, ":", &tok_end);

        while (token != NULL) {

            if (strlen(token) <= 3) {
                // Stores the UID assigned to the client
                uid = atoi(token);
                if (uid == 0) {
                    printf("\n=========================\nYOU ARE THE SUPERUSER\n====================\n");
                } else {
                    printf("\n=========================\nYOU ARE USER %d\n====================\n",uid);
                }
            } else {
                // Retrieves and stores all inputs already on the system
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
                // Adds the data to the worksheet grid
                acceptData(user_inpts[input_count].coord[0], user_inpts[input_count].coord[1]-'0', user_inpts[input_count].inp);
                input_count++;
            }
            token = strtok_r(NULL, ":", &tok_end);
        }
    }

// Thread to listen for updates from the server
void *server_updates(void *arg) {
        getNewSpreadsheet();
        updateClient((char *)(intptr_t)arg);
        drawWorksheet();
        if (uid == 0) {
            printf("\nENTER cell index to continue, 'quit' to exit or 'save' to save spreadsheet: \n");
        } else {
            printf("\nENTER cell index to continue or 'quit' to exit: \n");
        }
        while(1) {
            
            bytes_received = recv(sock_send,buf,BUF_SIZE,0);
            buf[bytes_received] = 0;
            printf("\nUpdate Received %d: %s \n",sock_send,buf);

            if (strcmp(buf, "quit") == 0) {
                bytes_sent=send(sock_send,buf,strlen(buf),0);
                close(sock_send);
                exit(0);
            } else {
                char *tok;
                char *token = strtok_r(buf, ",", &tok);
                int var = 0;
                while (token != NULL) {
                    if (var == 0) {
                        // Stores updated user count
                        user_count = atoi(token);
                    } else if (var == 1) {
                        // Stores cell coordinates of new input
                        strcpy(user_inpts[input_count].coord,token);
                    } else if (var == 2) {
                        // Stores user UID that made new input
                        user_inpts[input_count].user = atoi(token);
                    } else {
                        // Stores cell value
                        strcpy(user_inpts[input_count].inp,token);
                    }
                    token = strtok_r(NULL, ",", &tok);
                    var++;
                }

                // Updates worksheet grid
                acceptData(user_inpts[input_count].coord[0], user_inpts[input_count].coord[1]-'0', user_inpts[input_count].inp);
                input_count++;
                drawWorksheet();
                
                if (uid == 0) {
                    printf("\n=========================\nYOU ARE THE SUPERUSER\n========================\n");
                } else {
                    printf("\n=========================\nYOU ARE USER %d\n==========================\n",uid);
                }
                printf("\nNUMBER OF USERS: %d\n", user_count);
                printf("\nNUMBER OF INPUTS: %d\n", input_count);
                
                if (uid == 0) {
                    printf("\nENTER cell index to continue, 'quit' to exit or 'save' to save spreadsheet: \n");
                } else {
                    printf("\nENTER cell index to continue or 'quit' to exit: \n");
                }
            }

            
            
        }

        pthread_exit(&thread_id);
        close(sock_send);
        exit(0);
    }

void getInputType(char *res, char *val) {
    // Determines the type of input entered by the user
    // F: Function
    // T: Text
    // N: Real Number
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
    // Determines the type of funtion to be executed
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
    // Gets and checks the validity of the ranges of the function
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
        
        if (isOnWorksheet(cell[0], cell[1]-'0') == 1) {
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
    // Mapping of the letters and their associated index
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

    
    // Creation of thread to listen for updates from the server
    int thread = pthread_create(&thread_id, NULL, server_updates, (void *)(intptr_t)buf);
    if (thread != 0) {
        // If thread fails to be created the system will close
        printf("Failed to connect to server for updates\n");
        strcpy(buf,"quit");
        bytes_sent = send(sock_send,buf,strlen(buf),0);
        close(sock_send);
        exit(0);
    }

    
    void validateCellIndex(char *cell) {
        // To validate cell coodinated entered by the user
        while (1) {
            if (cell[0] >= 'a' && cell[0] <= 'z')
                cell[0] = cell[0] - 32;
            
            if (isOnWorksheet(cell[0], cell[1]-'0') ==  1)
                break;
            printf("Cell Index: ");
            scanf("%s",cell);
        }
        return;
    }

    while (1){
        if (uid == 0) {
            printf("\nENTER cell index to continue, 'quit' to exit or 'save' to save spreadsheet: \n");
        } else {
            printf("\nENTER cell index to continue or 'quit' to exit: \n");
        }

        scanf("%s",user_input);

        if (strcmp(user_input, "quit") == 0) {
            if (uid == 0) {
                // CLOSE ALL CLIENTS
                printf("Closing All Clients\n");
                strcpy(buf,"quit-all");
                bytes_sent=send(sock_send,buf,strlen(buf),0);
                printf("\nSENT %d: %s\n",bytes_sent,buf);
                break;
            } else {
                // CLOSE SELF ONLY
                pthread_cancel(thread_id);
                printf("Closing Self\n");
                strcpy(buf,"quit");
                bytes_sent=send(sock_send,buf,strlen(buf),0);
                printf("\nSENT %d: %s\n",bytes_sent,buf);
                break;
            }

        } else if (strcmp(user_input, "save") == 0) {
            // Sending command to save worksheet only works for user 0
            if (uid == 0) {
                bytes_sent=send(sock_send,user_input,strlen(user_input),0);
                printf("\nSENT %d: %s\n",bytes_sent,user_input);
            } else {
                printf("\n NO ACCESS SUPER USER ONLY \n");
            }
        } else {
            validateCellIndex(user_input);

            printf("Value: ");
            scanf("%s",cellVal);

            sprintf(text,"%d",uid);
            strcat(text,":");

            // Determines Input type
            getInputType(text, cellVal);

            inpType = text[strlen(text)-1];
            strcat(text,":");
            
            strcat(text,user_input);
            strcat(text,":");
            

            if (inpType == 'F') {
                // Determines function to be executed
                getFunctionType(text,cellVal);
                if (strcmp(text, "INVALID-FUNCTION") == 0) {
                    // If function is invalid
                    printf("-- %s\n", text);
                } else {
                    strcat(text,":");
                    // Determines validity of the range within the function
                    getFunctionRange(text, cellVal);
                    if (strcmp(text, "INVALID-RANGE-CELL") == 0) {
                        // If range is invalid
                        printf("-- %s\n", text);
                    } else {
                        // Sends the data to the server
                        strcpy(buf,text);
                        printf("\n%s\n",buf);
                        bytes_sent=send(sock_send,buf,strlen(buf),0);
                    }
                }
            } else {
                strcat(text,cellVal);
                strcpy(buf,text);
                // Sends the data to the server
                bytes_sent=send(sock_send,buf,strlen(buf),0);
                printf("\nSENT %d: %s\n",bytes_sent,buf);
            }
        }
    }
    
    pthread_join(thread_id, NULL);
    printf("DISCONNETCTED \n");
    close(sock_send);
    

    return 0; 
}

