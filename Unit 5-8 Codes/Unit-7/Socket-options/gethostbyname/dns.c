#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<errno.h>

#define MAXSIZE 90

void main()
{
    int sockfd, newsockfd;
    socklen_t len;
    struct sockaddr_in serveraddr, clientaddr;
    char buff[MAXSIZE];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(5353);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    listen(sockfd, 1);

    len = sizeof(clientaddr);
    newsockfd = accept(sockfd, (struct sockaddr*)&clientaddr, &len);

    recv(newsockfd, buff, sizeof(buff), 0);

    /* Simple static DNS mapping */
    if(strcmp(buff, "example.com") == 0)
        strcpy(buff, "192.168.1.10");
    else
        strcpy(buff, "0.0.0.0");

    send(newsockfd, buff, sizeof(buff), 0);

    close(newsockfd);
    close(sockfd);
}
