/*
 * TCP DNS Client
 * Reference: Lab No. 4, Exercise 1
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];
    char hostname[BUFFER_SIZE];

    // 1. Create Socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    // 2. Connect
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Localhost
    serv_addr.sin_port = htons(PORT);

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR connecting");
        exit(1);
    }

    printf("Connected to DNS Server.\n");

    while (1) {
        printf("\nEnter Hostname (e.g., www.google.com) or 'exit': ");
        fgets(hostname, BUFFER_SIZE, stdin);
        
        // Remove trailing newline
        hostname[strcspn(hostname, "\n")] = 0;

        // 3. Send Hostname
        write(sockfd, hostname, strlen(hostname));

        if (strcmp(hostname, "exit") == 0) {
            printf("Exiting...\n");
            break;
        }

        // 4. Receive IP Address
        bzero(buffer, BUFFER_SIZE);
        read(sockfd, buffer, BUFFER_SIZE);

        printf("Resolved IP Address: %s\n", buffer);
    }

    close(sockfd);
    return 0;
}