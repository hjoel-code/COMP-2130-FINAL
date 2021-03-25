#include <stdio.h>
#include <sys/types.h>	/* system type defintions */
#include <sys/socket.h>	/* network system functions */
#include <netinet/in.h>	/* protocol & struct definitions */
#include <stdlib.h>	/* exit() warnings */
#include <string.h>	/* memset warnings */
#include <unistd.h>
#include <pthread.h>

#define BUF_SIZE	1024
#define LISTEN_PORT	60000

int main(int argc, char *argv[]){

    int	sock_listen,sock_recv;
    struct sockaddr_in	my_addr,recv_addr;
    int i,addr_size,bytes_received;
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


    int searchUsers(int sock_recv) {
        printf("\nlooking %d\n", user_count);
        for (int n = 0; n <=(user_count);n++) {
            printf("\n%d : %d\n", users[n].sock_recv, sock_recv);
            if (users[n].sock_recv == sock_recv) {
                printf("\nFOUND\n");
                break;
            }
        }
        return 0;
    }

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
    i=bind(sock_listen, (struct sockaddr *) &my_addr, sizeof (my_addr));
    if (i < 0){
        printf("bind() failed\n");
        exit(0);
    }
    
    
    /* listen ... */
    
    i=listen(sock_listen, 5);
    if (i < 0){
        printf("listen() failed\n");
        exit(0);
    }
  /* get new socket to receive data on */
        /* It extracts the first connection request from the  */
        /* listening socket  */








    while (1) {
        addr_size=sizeof(recv_addr);
        sock_recv=accept(sock_listen, (struct sockaddr *) &recv_addr, &addr_size);
        

        if (fork() == 0) {
            close(sock_listen);
            users[user_count].sock_recv = sock_recv;
            user_count++;
            while (1) {
                bytes_received=recv(sock_recv,buf,BUF_SIZE,0);
                if (strcmp(buf,"quit") == 0)
                    exit(0);

                buf[bytes_received]=0;
                printf("Received %d: %s \n",sock_recv,buf);
                searchUsers(sock_recv);
            }
            close(sock_recv);
        } else {
            printf("\nNew Client\n");

            if (user_count > 0) {
                for (int i = 0; i <= user_count; i++) {
                    printf("%d\n", users[i].sock_recv);
                }
            }
            // Create new User Stucture
            // users[user_count].sock_recv = sock_recv;
            // user_count++;
            // Send UID to client side
            // if 1st client make super client
            
        }
    }

    return 0;
}

