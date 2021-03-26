#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define  PORT 4444

int  main ()
{
    int csocket,rec;
    struct sockaddr_in serverAddr;
    char buffer[1024];
    csocket=socket (AF_INET, SOCK_STREAM,0);
    if (csocket < 0)
    {
        printf("Error in connection.\n");
        exit(1);
    }

    printf("Client Socket is created.\n");

    memset(&serverAddr,'\0',sizeof(serverAddr));

    serverAddr.sin_family=AF_INET;
    serverAddr.sin_port=htons(PORT);
    serverAddr.sin_addr.s_addr=inet_addr("127.0.0.1");

    rec= connect(csocket, (struct sockaddr*)&serverAddr,sizeof(serverAddr));

   if (rec < 0)
    {
        printf("Error in connection.\n");
        exit(1);
    }
    
    printf("Conneccted to server.\n");
    
    while (1)
    {
        printf("Client: ");
        scanf("%s",&buffer[0]);
        send(csocket,buffer,strlen (buffer),0);

        if(strcmp(buffer,"stop")==0)
        {
            close(csocket);
            printf("\n Disconnected.\n");
            exit(1);
        } 
        
        if (recv(csocket,buffer,1024,0)<0)
        {
            printf("Error receiving info.\n");
            
        }
        else
        {
            printf("\nReceived.\n");
        }

    }
    return 0;
}