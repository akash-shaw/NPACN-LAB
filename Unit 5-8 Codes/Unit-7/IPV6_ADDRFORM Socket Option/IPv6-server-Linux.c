#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 12345

int main() {
    int server_sock, client_sock;
    struct sockaddr_in6 server_addr6;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[1024];

    // Create an IPv6 socket
    server_sock = socket(AF_INET6, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Convert the IPv6 socket to an IPv4 socket
    int family = AF_INET;
    if (setsockopt(server_sock, IPPROTO_IPV6, IPV6_ADDRFORM, &family, sizeof(family)) < 0) {
        perror("IPV6_ADDRFORM failed");
        exit(EXIT_FAILURE);
    }

    // Set server address structure (IPv4)
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind socket
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_sock, 5) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    // Accept client connection
    client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len);
    if (client_sock < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    // Receive data from client
    recv(client_sock, buffer, sizeof(buffer), 0);
    printf("Received: %s\n", buffer);

    // Close sockets
    close(client_sock);
    close(server_sock);
    return 0;
}
