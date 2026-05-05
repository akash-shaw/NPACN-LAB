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
    int recedbytes, sentbytes;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("\nSocket creation error");
        exit(0);
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(3380);
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    retval = connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    if (retval == -1) {
        printf("Connection error\n");
        close(sockfd);
        exit(0);
    }

    while (1) {
        printf("Enter message: ");
        fgets(buff, MAXSIZE, stdin);

        sentbytes = send(sockfd, buff, sizeof(buff), 0);
        if (sentbytes == -1) {
            close(sockfd);
            break;
        }

        memset(buff, '\0', sizeof(buff));
        recedbytes = recv(sockfd, buff, sizeof(buff), 0);
        if (recedbytes == -1) {
            close(sockfd);
            break;
        }
        printf("Server replied: %s\n", buff);
    }
    close(sockfd);
}