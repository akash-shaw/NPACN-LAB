#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080

int main() {
    int sockfd, newsockfd;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    char buffer[256];
    
    // 1. Create Socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }
    
    // 2. Bind
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);
    
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
        exit(1);
    }
    
    // 3. Listen
    listen(sockfd, 5);
    printf("TCP Server listening on port %d...\n", PORT);
    
    clilen = sizeof(cli_addr);
    
    // 4. Accept connection (This blocks until a client connects)
    // The cli_addr structure is filled with the client's address info here.
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0) {
        perror("ERROR on accept");
        exit(1);
    }
    
    // 5. Display Client Information
    printf("\n--- Client Connected ---\n");
    // Display IP Address
    printf("Client IP Address: %s\n", inet_ntoa(cli_addr.sin_addr));
    // Display Port Number
    printf("Client Port Number: %d\n", ntohs(cli_addr.sin_port));
    // Display Socket Family (Socket Address detail)
    printf("Client Socket Family: %d (AF_INET)\n", cli_addr.sin_family);
    
    close(newsockfd);
    close(sockfd);
    return 0;
}