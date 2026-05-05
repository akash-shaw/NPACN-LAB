#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netinet/tcp.h>

#define MAXSIZE 90

void main()
{
    int sockfd, sentbytes, recedbytes;
    struct sockaddr_in serveraddr;
    char buff[MAXSIZE];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    {
        printf("Socket error\n");
        exit(1);
    }

    int opt = 1;

    /* TCP options */
    setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt));
    setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));
    

    int mss = 1460;
    setsockopt(sockfd, IPPROTO_TCP, TCP_MAXSEG, &mss, sizeof(mss));

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(3380);
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));

    strcpy(buff, "Hello from TCP client");
    sentbytes = send(sockfd, buff, strlen(buff)+1, 0);

    recedbytes = recv(sockfd, buff, sizeof(buff), 0);
    puts(buff);

    close(sockfd);
}
