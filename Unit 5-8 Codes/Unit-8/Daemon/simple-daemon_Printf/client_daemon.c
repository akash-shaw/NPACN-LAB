#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXSIZE 90

int main() {
    int sockfd;
    struct sockaddr_in serveraddr;
    char buff[MAXSIZE] = "Hello Daemon!";

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(3380);
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) == 0) {
        send(sockfd, buff, strlen(buff), 0);
        recv(sockfd, buff, MAXSIZE, 0);
        printf("Response from Daemon: %s\n", buff);
    } else {
        printf("Connection failed. Is the daemon running?\n");
    }

    close(sockfd);
    return 0;
}