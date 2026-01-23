#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    char buffer[BUFFER_SIZE];
    char input[BUFFER_SIZE];
    struct sockaddr_in server_addr;

    // 1. Create socket file descriptor (SOCK_DGRAM for UDP)
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));

    // 2. Fill server information
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY; 

    while (1) {
        printf("\nEnter a string to find permutations (or 'exit' to quit): ");
        scanf("%s", input);

        if (strcmp(input, "exit") == 0) {
            break;
        }

        // 3. Send message to server
        sendto(sockfd, (const char *)input, strlen(input), MSG_CONFIRM, 
               (const struct sockaddr *)&server_addr, sizeof(server_addr));

        printf("String sent. Waiting for server acknowledgement...\n");

        // 4. Receive acknowledgement (Half-Duplex: Block until server replies)
        socklen_t len = sizeof(server_addr);
        int n = recvfrom(sockfd, (char *)buffer, BUFFER_SIZE, MSG_WAITALL, 
                         (struct sockaddr *)&server_addr, &len);
        buffer[n] = '\0';
        
        printf("Server replied: %s\n", buffer);
    }

    close(sockfd);
    return 0;
}