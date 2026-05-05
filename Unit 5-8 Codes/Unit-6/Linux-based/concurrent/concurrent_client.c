#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXSIZE 90

void main() {
    int sockfd, retval;
    struct sockaddr_in serveraddr;
    char buff[MAXSIZE];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(3380);
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    retval = connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    if (retval == -1) {
        printf("Connection error\n");
        exit(0);
    }

    while (1) {
        printf("Enter message: ");
        fgets(buff, MAXSIZE, stdin);
        send(sockfd, buff, sizeof(buff), 0);

        if (strncmp(buff, "quit", 4) == 0) break;

        memset(buff, '\0', sizeof(buff));
        recv(sockfd, buff, sizeof(buff), 0);
        printf("Server: %s\n", buff);
    }
    close(sockfd);
}