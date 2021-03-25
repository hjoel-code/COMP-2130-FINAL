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

// Global Variables:
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

// Client Thread acts as listener for a specific client in various threads
void *client_thread(void *arg)
{
	users[user_count].sock_recv = sock_recv;
    user_count++;
    while (1) {
        bytes_received=recv(sock_recv,buf,BUF_SIZE,0);
        if (strcmp(buf,"quit") == 0)
            break;

        buf[bytes_received]=0;
        printf("Received %d: %s \n",sock_recv,buf);
        searchUsers(sock_recv);
    }
    close(sock_recv);
}

int main()
{
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
        
        if (pthread_create(&thread_id, NULL, client_thread, NULL) != 0) {
            printf("Failed to connect client\n");
        } else {
            printf("New Client\n");
        }

    }
    close(sock_listen);
	pthread_join(thread_id, NULL);

    return 0;
}
