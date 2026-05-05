#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[1024] = {0};
    char *message = "Hello from IPv4 client!";

    // Create an IPv4 socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    // Convert and set IPv4 address (Assuming server is on localhost)
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("Invalid IPv4 address");
        exit(EXIT_FAILURE);
    }

    // Connect to the IPv6 server (IPv4-mapped)
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Connected to the server!\n");

    // Send message to server
    send(sockfd, message, strlen(message), 0);

    // Receive response from server
    read(sockfd, buffer, sizeof(buffer));
    printf("Server response: %s\n", buffer);

    // Close the socket
    close(sockfd);

    return 0;
}
