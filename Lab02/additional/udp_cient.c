/*
 * UDP Client
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8888
#define BUFFER_SIZE 4096

int main() {
    int sockfd;
    char buffer[BUFFER_SIZE];
    char filename[100];
    struct sockaddr_in servaddr;

    // 1. Create Socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // Server Info
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    socklen_t len;
    int n;

    while (1) {
        printf("\nEnter filename (or 'stop' to exit): ");
        scanf("%s", filename);

        // Send Filename
        sendto(sockfd, (const char *)filename, strlen(filename), MSG_CONFIRM, (const struct sockaddr *)&servaddr, sizeof(servaddr));

        if (strcmp(filename, "stop") == 0) {
            break;
        }

        // Receive Response
        len = sizeof(servaddr);
        memset(buffer, 0, BUFFER_SIZE);
        n = recvfrom(sockfd, (char *)buffer, BUFFER_SIZE, MSG_WAITALL, (struct sockaddr *)&servaddr, &len);
        buffer[n] = '\0';

        printf("%s\n", buffer);
    }

    close(sockfd);
    return 0;
}