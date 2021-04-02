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

//global declaration structure grid
char * grid[NUM_RANGE][NUM_RANGE];

// Global Variables:
int	sock_listen,sock_recv;
struct sockaddr_in	my_addr,recv_addr;
int i,addr_size,bytes_received;
int	send_len,bytes_sent;
int	incoming_len;
struct sockaddr	remote_addr;
int	recv_msg_size;
char buf[BUF_SIZE];
int close_sock = 0;

struct User {
    int sock_recv;
    // List of cells adjusted
};

struct User users[20];
int user_count = 0;

struct UserInput {
    char coord[2];
    int user;
    char inp[10];
};

struct UserInput user_inpts[82];
int input_count = 0;

struct AlphIndex {
    // Dictionary Structure to store the letter and score associated with a letter
    char alph;
    int index;
};

char alph[] = "ABCDEFGHI"; 
int idx[] = {0,1,2,3,4,5,6,7,8};

struct AlphIndex alphIndex[NUM_RANGE]; 

int searchIndex(char letter) {
    for (int n=0;n<NUM_RANGE;n++) {
        if (alphIndex[n].alph == letter) {
            return alphIndex[n].index;
        }
    }
    return 0;
}

void getNewBoard(){
            //creates a brand new blank board. Returns a pointer to the array 
            for (int j = 0; j < NUM_RANGE; j++){
                for (int k = 0; k < NUM_RANGE; k++)
                {
                    grid[k][j]="   ";
                }//Endfor
            }//Endfor
            return;
        }//EndFunction getNewBoard

void makePlay(char alph, int y, char* c){
        printf("\n%c - %d\n",alph,y-1);
        int x = searchIndex(alph);
        grid[x][y-1]=c;
        return;
    }

int searchUsers(int sock_recv) {
        for (int n = 0; n <=(user_count);n++) {
            if (users[n].sock_recv == sock_recv) {
                printf("FOUND\n");
                return n;
            }
        }
        return -1;
    }

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
            k = atof(grid[n][m]);
            printf("GRID[%d][%d]: %s\n",n,m,grid[n][m]);
            if (k != 0) {
                strcat(range_values, grid[n][m]);
                strcat(range_values, ",");
            }
        }
    }
    return;
}


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
    float avg = sum / (count-1);
    printf("%f\n",avg);
    return avg;
}

void updateGrid(char *msg) {
    printf("Message to send%s\n", msg);
    char cell[3];
    char val[20];
    char *tok;
    char *token = strtok_r(msg,":",&tok);
    int trk = 0;
    char range[6], type;



    while (token != NULL) {
        if (trk == 0) {
            user_inpts[input_count].user = atoi(token);
        } else if (trk == 1) {
            type = token[0];
            printf("%s\n",token);
        } else if (trk == 2) {
            strcpy(cell,token);
        } else if (trk == 3) {
            strcpy(val,token);
            printf("%s\n", token);
        } else if (trk == 4) {
            printf("%s\n", token);
            strcpy(range,token);
        }
        token = strtok_r(NULL,":",&tok);
        trk++;
    }
    strcpy(user_inpts[input_count].coord,cell);
    if (type == 'F') {
        char *cells;
        char cell1[2], cell2[2];
        int c_count = 0;

        cells = strtok(range,",");
        while (cells != NULL) {
            if (c_count == 0) {
                strcpy(cell1,cells);
                printf("Cell 1: %s\n", cell1);
            } else if (c_count == 1) {
                strcpy(cell2,cells);
                printf("Cell 2: %s\n", cell2);
            }
            cells = strtok(NULL, ",");
            c_count++;
        }

        char range_vals[2*((NUM_RANGE*NUM_RANGE)-1)];
        
        getRange(range_vals, cell1, cell2);
        printf("--- %s\n", range_vals);

        if (val[0] == 'S') {
            sprintf(user_inpts[input_count].inp, "%.2f", sumRange(range_vals)); 
            printf("FIND SUM\n");
        } else if (val[0] == 'R') {
            sprintf(user_inpts[input_count].inp, "%.2f", sumRange(range_vals)); 
            printf("FIND RANGE\n");
        } else if (val[0] == 'A') {
            sprintf(user_inpts[input_count].inp, "%.2f", avgRange(range_vals)); 
            printf("FIND AVERAGE\n");
        }

        printf("Cell ---- %s\n", user_inpts[input_count].coord);
        makePlay(user_inpts[input_count].coord[0], user_inpts[input_count].coord[1]-'0', user_inpts[input_count].inp);
    } else {
        strcpy(user_inpts[input_count].inp,val);
        makePlay(user_inpts[input_count].coord[0], user_inpts[input_count].coord[1]-'0', user_inpts[input_count].inp);
    }
    

    char ch[3];
    char buf[40];
    strcpy(buf,"");
    strcat(buf, user_inpts[input_count].coord);
    strcat(buf,",");
    sprintf(ch, "%d", user_inpts[input_count].user);
    strcat(buf, ch);
    strcat(buf,",");
    strcat(buf, user_inpts[input_count].inp);
    input_count++;

    printf("To send %d: %s\n", user_count, buf);

    for (int i = 0; i < user_count; i++) {
        write(users[i].sock_recv, buf, strlen(buf));
        printf("\n%d - %d :Send\n",i,users[i].sock_recv);
    }

    return;
}



// Client Thread acts as listener for a specific client in various threads
void *client_thread(void *arg) {   
    int sock = (int)(intptr_t)arg;
	users[user_count].sock_recv = sock;
    printf("\n %d - %d : Sock = %d \n",user_count,users[user_count].sock_recv,sock);
    user_count++; 
    while (1) {
        bytes_received=recv(sock,buf,BUF_SIZE,0);
        if (strcmp(buf,"quit") == 0)
            break;

        buf[bytes_received]=0;
        printf("Received %d: %s \n",sock,buf);
        updateGrid(buf);

    }
    close(sock);
}

int main()
{
    getNewBoard();
    // Mapping of the letters and their associated scores
    for (int j=0;j<NUM_RANGE;j++) {
        alphIndex[j].alph = alph[j];
        alphIndex[j].index = idx[j];
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

	pthread_t thread_id;
	printf("Pending Connection\n");

    while (1) {
        addr_size=sizeof(recv_addr);
        sock_recv=accept(sock_listen, (struct sockaddr *) &recv_addr, &addr_size);
        
        if (pthread_create(&thread_id, NULL, client_thread, (void *)(intptr_t)sock_recv) != 0) {
            printf("Failed to connect client\n");
        } else {
            printf("New Client %d\n", input_count);
            char ch[3];
            sprintf(buf, "%d", user_count);
            if (input_count > 0) {
                for (int i = 0; i < input_count; i++) {
                    strcat(buf,":");
                    strcat(buf, user_inpts[i].coord);
                    strcat(buf,",");
                    sprintf(ch, "%d", user_inpts[i].user);
                    strcat(buf, ch);
                    strcat(buf,",");
                    strcat(buf, user_inpts[i].inp);
                }
            }
            write(sock_recv, buf, strlen(buf));
        }
    }

    close(sock_listen);
	pthread_join(thread_id, NULL);

    return 0;
}
