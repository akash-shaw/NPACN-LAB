#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#define MAXSIZE 90

int main()
{
    int sockfd;
    struct sockaddr_in serveraddr;
    char buff[MAXSIZE] = "Hello from client";

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0)
    {
        perror("Socket error");
        exit(1);
    }

    int ttl = 64;
    int tos = 16;

    setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl));
    setsockopt(sockfd, IPPROTO_IP, IP_TOS, &tos, sizeof(tos));

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(3380);
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    sendto(sockfd, buff, strlen(buff)+1, 0,
           (struct sockaddr*)&serveraddr, sizeof(serveraddr));

    close(sockfd);
    return 0;
}
