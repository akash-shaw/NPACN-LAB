#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include <errno.h>

#define MAXSIZE 90

void main() {
    int sockfd, retval;
    struct sockaddr_in serveraddr;
    char buff[MAXSIZE];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1) {
        printf("\nSocket creation error");
        exit(1);
    }

    // --- SO_LINGER IMPLEMENTATION (Row 4 of your table) ---
    struct linger sl;
    sl.l_onoff = 1;  // Enable lingering
    sl.l_linger = 0; // Set timeout to 0 for an immediate RST/Abort
    setsockopt(sockfd, SOL_SOCKET, SO_LINGER, &sl, sizeof(sl));

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(3380);
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    retval = connect(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if(retval == -1) {
        perror("Connect error");
        close(sockfd);
        exit(1);
    }

    printf("Sending greeting to server...\n");
    strcpy(buff, "Hello Server!");
    send(sockfd, buff, strlen(buff), 0);

    memset(buff, 0, MAXSIZE);
    recv(sockfd, buff, sizeof(buff), 0);
    printf("Server said: %s\n", buff);

    printf("Closing socket with SO_LINGER (RST)...\n");
    close(sockfd); // This triggers the RST because l_linger = 0
}