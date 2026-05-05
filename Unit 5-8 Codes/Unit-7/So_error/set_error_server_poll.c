#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#define MAXSIZE 90

int main() {
    int sockfd, newsockfd, retval;
    socklen_t actuallen;
    struct sockaddr_in serveraddr, clientaddr;
    char buff[MAXSIZE];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation error");
        exit(1);
    }

    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(3380);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0) {
        perror("Binding error");
        close(sockfd);
        exit(1);
    }

    listen(sockfd, 1);
    printf("Server listening on port 3380...\n");

    actuallen = sizeof(clientaddr);
    newsockfd = accept(sockfd, (struct sockaddr*)&clientaddr, &actuallen);
    if (newsockfd == -1) {
        perror("Accept error");
        close(sockfd);
        exit(1);
    }

    memset(buff, 0, MAXSIZE);
    if (recv(newsockfd, buff, sizeof(buff), 0) > 0) {
        printf("Received from client: %s", buff);
        printf("Enter response: ");
        fgets(buff, MAXSIZE, stdin);
        send(newsockfd, buff, strlen(buff), 0);
    }

    close(newsockfd);
    close(sockfd);
    return 0;
}