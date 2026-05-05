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
    int sockfd, newsockfd, retval;
    socklen_t actuallen;
    int recedbytes, sentbytes;
    struct sockaddr_in serveraddr, clientaddr;
    char buff[MAXSIZE];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1) {
        printf("\nSocket creation error");
        exit(1);
    }

    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(3380);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    retval = bind(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if(retval < 0) {
        perror("Binding error");
        close(sockfd);
        exit(1);
    }

    listen(sockfd, 1);
    printf("Server listening on 3380...\n");

    actuallen = sizeof(clientaddr);
    newsockfd = accept(sockfd, (struct sockaddr*)&clientaddr, &actuallen);

    // Receive initial data
    recedbytes = recv(newsockfd, buff, sizeof(buff), 0);
    if(recedbytes > 0) {
        printf("Client: %s\n", buff);
    }

    // --- SHUTDOWN IMPLEMENTATION (Row 2 of your table) ---
    printf("Sending response and performing SHUT_WR...\n");
    strcpy(buff, "Goodbye Client!");
    send(newsockfd, buff, strlen(buff), 0);
    
    // Process can no longer send, but FIN is sent to client
    shutdown(newsockfd, SHUT_WR); 

    // Because it's SHUT_WR, we can STILL receive if the client sends more
    recedbytes = recv(newsockfd, buff, sizeof(buff), 0);
    if(recedbytes == 0) {
        printf("Client closed connection.\n");
    }

    close(newsockfd);
    close(sockfd);
}