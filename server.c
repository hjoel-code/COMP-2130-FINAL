#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define  PORT 4444

int main()
{
    int serversoc,srec;
    struct sockaddr_in serverAddr;

    int newsocket;
    struct sockaddr_in newAddr;
    char buffer[1024];
    pid_t cpid;

    socklen_t addr_size;

    serversoc=socket(AF_INET,SOCK_STREAM,0);

      if (serversoc < 0)
    {
        printf("Error in connection.\n");
        exit(1);
    }
    printf("Server Socket is created.\n");

    memset(&serverAddr,'\0',sizeof(serverAddr));
    serverAddr.sin_family=AF_INET;
    serverAddr.sin_port=htons(PORT);
    serverAddr.sin_addr.s_addr=inet_addr("127.0.0.1");

    srec=bind(serversoc,(struct sockaddr*)&serverAddr,sizeof(serverAddr));
    if (srec < 0)
    {
        printf("Error in binding.\n");
        exit(1);
    }

    printf("Bind to port %d.\n",4444);

    if (listen(serversoc,10)== 0)
    {
        printf("Listening...\n");
    }
    else
    {
        printf("Error in binding.\n");
    }
    
    while (1)
    {
        newsocket=accept(serversoc,(struct sockaddr*)&newAddr,&addr_size);
        if(newsocket<0)
        {
            exit(1);
        }
        printf("Connection acceptedd from %s:%d\n",inet_ntoa(newAddr.sin_addr),ntohs(newAddr.sin_port));
        if ((cpid=fork())==0)
        {
            close(serversoc);
            while(1)
            {
                recv(newsocket,buffer,1024,0);
                if(strcmp(buffer,"stop")==0)
                {
                    printf("Disconnetd from %s:%d\n",inet_ntoa(newAddr.sin_addr),ntohs(newAddr.sin_port));
                    break;
                }
                else
                {
                    printf("Client: %s\n",buffer);
                    send(newsocket,buffer,strlen(buffer),0);
                    bzero(buffer,sizeof(buffer));
                }
            }
        }
    }
    close(newsocket);
    return 0;
}