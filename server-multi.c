#include <stdio.h>
#include <sys/types.h>	/* system type defintions */
#include <sys/socket.h>	/* network system functions */
#include <netinet/in.h>	/* protocol & struct definitions */
#include <stdlib.h>	/* exit() warnings */
#include <string.h>	/* memset warnings */
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include <pthread.h>


#define BUF_SIZE	1024
#define LISTEN_PORT	60000
#define NUM_RANGE 9

//global declaration grid structure
char * grid[NUM_RANGE][NUM_RANGE];

// Global Variables:

// ========= GLOBAL SOCKET VARIABLES ================
int	sock_listen,sock_recv;
struct sockaddr_in	my_addr,recv_addr;
int i,addr_size,bytes_received;
int	send_len,bytes_sent;
int	incoming_len;
struct sockaddr	remote_addr;
int	recv_msg_size;
char buf[BUF_SIZE];
int close_sock = 0;
// ==================================================

// Stroes the thread ID
pthread_t thread_id; 

// Declaration of user structure
struct User {
    int sock_recv;
};

// Initialization of user structure and user count
struct User users[20];
int user_count = 0;


// Declaration of spreadsheet input structure
struct UserInput {
    char coord[2];
    int user;
    char inp[10];
};

// Initialization of input structure and input count
struct UserInput user_inpts[82];
int input_count = 0;

// Declaration of grid's columns structure
struct AlphIndex {
    // Stores grid letter to corresponding index
    char alph;
    int index;
    int width; //Stores width of each column on the grid
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

// Creates a new Spreadsheet
void getNewSpreadsheet(){
        //creates a brand new spreadsheet. Returns a pointer to the array 
        for (int j = 0; j < NUM_RANGE; j++){
            for (int k = 0; k < NUM_RANGE; k++)
            {
                grid[k][j]="   ";
            }//Endfor
        }//Endfor
        return;
    }//EndFunction getNewSpreadsheet

// Accepts cell index and cell value to be stored
// and adds it to the grid
void acceptData(char alph, int y, char* c){
        int x = searchIndex(alph); // Returns corresponding index to Character
        int len = strlen(c);

        if (alphIndex[x].width < len)
            alphIndex[x].width = len;

        grid[x][y-1]=c;
        return;
    }

// Searches through list of users for corresponding socket number
int searchUsers(int sock) {
        for (int n = 0; n <=(user_count);n++) {
            if (users[n].sock_recv == sock) {
                printf("FOUND\n");
                return n;
            }
        }
        return -1;
    }

// Removes user from list of users
void removeUser(int user_index) {
    for (int n = user_index; n < user_count; n++) {
        users[n] = users[n+1];
    }
    user_count--;
    return;
}

// Returns all intergers and real numbers within the specified range
// Values are stored to the range_values pointer location
void getRange(char *range_values, char *range1, char *range2) {
    int x1,x2,y1,y2;

    int tmp;

    float k;

    x1 = searchIndex(range1[0]);
    x2 = searchIndex(range2[0]);

    if (x1 > x2) {
        tmp = x1;
        x1 = x2;
        x2 = tmp;
    }

    y1 = (range1[1] - '0')-1;
    y2 = (range2[1] - '0')-1;

    if (y1 > y2) {
        tmp = y1;
        y1 = y2;
        y2 = tmp;
    }

    strcpy(range_values, "");
    for (int n=x1; n<=x2; n++) {
        for (int m=y1; m<=y2; m++) {
            // Checks if grid value is a floating number
            k = atof(grid[n][m]); 
            if (k != 0) {
                // If grid value is a floating number the value is added to the character pointer
                strcat(range_values, grid[n][m]);
                strcat(range_values, ",");
            }
        }
    }
    return;
}


// ============ REQUIRED FUNCTIONS =============================

// Returns the sum of the range of values in the character pointer 
float sumRange(char *range_vals) {
    char *val;

    val = strtok(range_vals, ",");
    float sum = 0.00;
    while (val != NULL) {
        sum += atof(val);
        val = strtok(NULL, ",");
        printf("%f\n",sum);
    }
    printf("%f\n",sum);
    return sum;
}

// Returns the average of the range of values in the character pointer 
float avgRange(char *range_vals) {
    char *val;

    val = strtok(range_vals, ",");
    int count = 0;
    float sum = 0.00;
    while (val != NULL) {
        sum += atof(val);
        val = strtok(NULL, ",");
        printf("%f\n",sum);
        count++;
    }
    float avg = sum / count;
    printf("%f\n",avg);
    return avg;
}

// Returns the difference between the largest
// and the smallest values within the range of values 
// in the character pointer 
float rrange(char *range_vals) {
    char *num;
    float temp,temp2,ans=0;

    num = strtok(range_vals,",");

    temp=0;
    temp2 = atof(num);
    while (num != NULL)
    {
        if  (temp < (atof(num))){
            temp=atof(num);
        }
        else if ((atof(num) < temp2)){
            temp2= atof(num);
        }

        num = strtok(NULL,",");
    }

    ans=temp-temp2;
    return ans;
}

// ================================================================



// Accepts input data from the client socket and updates the speadsheet
void updateGrid(char *msg) {
    char cell[3];
    char val[20];
    
    char *tok;
    char *token = strtok_r(msg,":",&tok);
    int trk = 0;
    
    char range[6], type;



    while (token != NULL) {
        if (trk == 0) {
            // Stores the user/client updating the spreadsheet
            user_inpts[input_count].user = atoi(token);
        } else if (trk == 1) {
            // Stores the Type of input being made by the client
            type = token[0];
        } else if (trk == 2) {
            // Stores the cell of input
            strcpy(cell,token);
        } else if (trk == 3) {
            // Stores the value being entered into the spreadsheet
            // or if a function stores the type of function A, S or R
            strcpy(val,token);
            printf("%s\n", token);
        } else if (trk == 4) {
            // Stores the range of cells of the given function
            printf("%s\n", token);
            strcpy(range,token);
        }
        token = strtok_r(NULL,":",&tok);
        trk++;
    }
    strcpy(user_inpts[input_count].coord,cell);
    if (type == 'F') {
        // If input is a function
        char *cells;
        char cell1[2], cell2[2];
        int c_count = 0;

        cells = strtok(range,",");
        while (cells != NULL) {
            if (c_count == 0) {
                // Stores the first cell of 2 within the range
                strcpy(cell1,cells);
                printf("Cell 1: %s\n", cell1);
            } else if (c_count == 1) {
                // Stores the second cell of 2 within the range
                strcpy(cell2,cells);
                printf("Cell 2: %s\n", cell2);
            }
            cells = strtok(NULL, ",");
            c_count++;
        }

        //  Varible to store all values within specified range
        char range_vals[2*((NUM_RANGE*NUM_RANGE)-1)]; 
        
        //  Gets all Range values that are real numbers from the spreadsheet
        getRange(range_vals, cell1, cell2);
        
        printf("--- %s\n", range_vals);

        if (val[0] == 'S') {
            // Calculates and stores the sum of all values within the range
            sprintf(user_inpts[input_count].inp, "%.2f", sumRange(range_vals)); 
            printf("FIND SUM\n");
        } else if (val[0] == 'R') {
            // Calculates and stores the diff between the largest and smallest values 
            // of all values within the range
            sprintf(user_inpts[input_count].inp, "%.2f", rrange(range_vals)); 
            printf("FIND RANGE\n");
        } else if (val[0] == 'A') {
            // Calculates and stores the average of all values within the range
            sprintf(user_inpts[input_count].inp, "%.2f", avgRange(range_vals)); 
            printf("FIND AVERAGE\n");
        }
        // Adds data to the spreadsheet
        acceptData(user_inpts[input_count].coord[0], user_inpts[input_count].coord[1]-'0', user_inpts[input_count].inp);
    } else {

        // Adds data to the spreadsheet
        strcpy(user_inpts[input_count].inp,val);
        acceptData(user_inpts[input_count].coord[0], user_inpts[input_count].coord[1]-'0', user_inpts[input_count].inp);
    }
    

    char ch[3];
    char buf[40];
    char count_user[3];


    //  ====== Creation of message to be sent to each client =========
    sprintf(count_user,"%d",user_count);
    strcpy(buf, count_user);
    strcat(buf,",");
    strcat(buf, user_inpts[input_count].coord);
    strcat(buf,",");
    sprintf(ch, "%d", user_inpts[input_count].user);
    strcat(buf, ch);
    strcat(buf,",");
    strcat(buf, user_inpts[input_count].inp);
    //  ===========================================================
    input_count++;

    printf("To send %d: %s\n", user_count, buf);

    // Sending the updates to all clients connected to the system
    for (int i = 0; i < user_count; i++) {
        write(users[i].sock_recv, buf, strlen(buf));
        printf("\nSENT TO U%d: %s\n", i,buf);
    }

    return;
}


// Send quit command to all clients
void closeClients() {
    strcpy(buf, "quit");
    for (int i = 0; i < user_count; i++) {
        write(users[i].sock_recv, buf, strlen(buf));
        printf("\n%d - %d :Send\n",i,users[i].sock_recv);
    }
}

// Saves current worksheet to file
void saveWorksheet()
{   //SAVING WORKSHEET TO FILE
    FILE *ptr;
    
    ptr=fopen ("Worksheet","w");
    
    if (ptr==NULL){
        printf("error");
        exit(1);
    }
    printf("\nSaving...\n");
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

    fprintf(ptr,"%s\n",NLINE);
    fprintf(ptr,"%s\n",HLINE);
    for (int j = 0; j < NUM_RANGE; j++)
    {  
        fprintf(ptr,"%s\n",VLINE);
        fprintf(ptr,"%d ",j+1);
        for (int k = 0; k < NUM_RANGE; k++)
        {  
            if(strcmp(grid[k][j],"   ")==0){
                char space[40];
                strcpy(space,"   ");
                 for (int m = 0; m <= alphIndex[k].width-2; m++) {
                    strcat(space," ");
                }
                fprintf(ptr,"| %s",space);
            }else{
                if (alphIndex[k].width > strlen(grid[k][j])) {
                    int dif = alphIndex[k].width - strlen(grid[k][j]);
                    char input[30];
                    strcpy(input,"");
                    for (int m = 0; m <= dif-1; m++) {
                        strcat(input," ");
                    }
                    strcat(input,grid[k][j]);
                    fprintf(ptr,"| %s  ",input);
                } else {
                    fprintf(ptr,"| %s  ",grid[k][j]);
                }
            }
        }
        fprintf(ptr,"%s","|");
        fprintf(ptr,"\n");
        fprintf(ptr,"%s\n",VLINE);
        fprintf(ptr,"%s\n",HLINE);
    }
    fclose(ptr);
    return;
}


// Client Thread acts as listener for a specific client in various threads
void *client_thread(void *arg) {
    
    int sock = (int)(intptr_t)arg;
	users[user_count].sock_recv = sock;
    user_count++; 
    while (1) {
        strcpy(buf,"");
        bytes_received=recv(sock,buf,BUF_SIZE,0);
        buf[bytes_received]=0;
        printf("Received %d - %d: %s \n",sock,bytes_received,buf);
        
        if (strcmp(buf,"quit") == 0) {
            // Closes the socket between client and server
            int user_index = searchUsers(sock);
            if (user_index == -1) {
                printf("USER NOT FOUND\n");
            } else {
                removeUser(user_index);
            }
            printf("User count %d\n", user_count);
            break;
        } else if (strcmp(buf, "quit-all") == 0) {
            // Closes the socket between all client and server
            // and closes the system
            closeClients();
            close(sock);
            user_count--;
            printf("User count %d\n", user_count);
            break;
        } else if (strcmp(buf, "save") == 0) {
            // Saves the worksheet to file
            printf("\n SAVING SPREADSHEET\n");
            saveWorksheet();
            printf("\nSAVED\n");
        } else {
            // Adds data to the worksheet
            buf[bytes_received]=0;
            updateGrid(buf);
        }
    }
    
    printf("DISCONNETCTED \n"); 
    close(sock);

    

    if (user_count == 0) {
        // If no users are left on the system the all sockets will be closed
        // and the program will end
        printf("\nNO CLIENTS -- CLOSING SERVER\n");
        close(sock_listen);
        pthread_join(thread_id, NULL);
        exit(0);
    }

    pthread_exit(&thread_id);

}

int main()
{

    getNewSpreadsheet();


    for (int j=0;j<NUM_RANGE;j++) {
        alphIndex[j].alph = alph[j];
        alphIndex[j].index = idx[j];
        alphIndex[j].width = 1;
    };

    /* create socket for listening */
    sock_listen=socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    if (sock_listen < 0){
        printf("socket() failed\n");
        exit(0);
    }

    /* make local address structure */
    memset(&my_addr, 0, sizeof (my_addr));	/* zero out structure */
    my_addr.sin_family = AF_INET;	/* address family */
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);  /* current machine IP */
    my_addr.sin_port = htons((unsigned short)LISTEN_PORT);


    /* bind socket to the local address */
    if (bind(sock_listen, (struct sockaddr *) &my_addr, sizeof (my_addr)) < 0){
        printf("bind() failed\n");
        exit(0);
    }
    

    /* listen ... */ 
    if (listen(sock_listen, 5) < 0){
        printf("listen() failed\n");
        exit(0);
    }

	
	printf("Pending Connection\n");

    while (1) {
        addr_size=sizeof(recv_addr);
        sock_recv=accept(sock_listen, (struct sockaddr *) &recv_addr, &addr_size);
        
        if (pthread_create(&thread_id, NULL, client_thread, (void *)(intptr_t)sock_recv) != 0) {
            printf("Failed to connect client\n");
        } else {
            // Occurs if a New Client connects to the system
            printf("New Client %d\n", user_count);
            char ch[3];
            sprintf(buf, "%d", user_count);
            if (input_count > 0) {
                // Creation of message containing information of all inputs on the worksheet =======
                for (int i = 0; i < input_count; i++) {
                    strcat(buf,":");
                    strcat(buf, user_inpts[i].coord);
                    strcat(buf,",");
                    sprintf(ch, "%d", user_inpts[i].user);
                    strcat(buf, ch);
                    strcat(buf,",");
                    strcat(buf, user_inpts[i].inp);
                }
                // ===================================================================================
            }
            // Sends all inputs that are in the system to the new client
            write(sock_recv, buf, strlen(buf));
        }
        
    }
    close(sock_listen);
    pthread_join(thread_id, NULL);
	

    return 0;
}
