/*
 * Lab 5: Client
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8888
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];
    char input[BUFFER_SIZE];

    // 1. Create Socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    // 2. Connect
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        exit(1);
    }

    // Check if Server rejected connection immediately (Max clients reached)
    // We do a non-blocking check or just proceed to input. 
    // Usually, the server accepts, then we write. 
    // However, if the server is full, it writes "terminate session" immediately.
    // Let's prompt user first as per standard flow, but be ready to read.

    printf("Enter string to send (e.g., 'Institute Of' or 'Technology'): ");
    fgets(input, BUFFER_SIZE, stdin);
    input[strcspn(input, "\n")] = 0; // Remove newline

    // 3. Send String
    write(sockfd, input, strlen(input));

    // 4. Check for Termination Message (Optional - if client needs to know)
    memset(buffer, 0, BUFFER_SIZE);
    // Give server a moment to respond if it's the termination case
    // In normal case, server closes. In >2 case, server sends data then closes.
    int n = read(sockfd, buffer, BUFFER_SIZE);
    
    if (n > 0) {
        if (strstr(buffer, "terminate session") != NULL) {
            printf("Server sent: %s. Exiting.\n", buffer);
        } else {
             printf("Server acknowledged.\n");
        }
    } else {
        printf("Data sent. Connection closed by server.\n");
    }

    close(sockfd);
    return 0;
}