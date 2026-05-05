#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>

#define SERVER_IP "127.0.0.1" // IPv4 server address
#define SERVER_PORT 8080

int main() {
    int sock;
    struct sockaddr_in6 server_addr;
    char buffer[1024] = "Hello from IPv6 client!";

    // Create IPv6 socket
    sock = socket(AF_INET6, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Allow IPv6 socket to connect to IPv4-mapped addresses
    int disable_v6only = 0;
    setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, &disable_v6only, sizeof(disable_v6only));

    // Configure server address (IPv6)
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin6_family = AF_INET6;
    server_addr.sin6_port = htons(SERVER_PORT);

    // Convert IPv4 address to IPv4-mapped IPv6 address (::ffff:<IPv4>)
    struct in_addr ipv4_addr;
    inet_pton(AF_INET, SERVER_IP, &ipv4_addr);
    server_addr.sin6_addr.s6_addr[10] = 0xff;
    server_addr.sin6_addr.s6_addr[11] = 0xff;
    memcpy(&server_addr.sin6_addr.s6_addr[12], &ipv4_addr, sizeof(ipv4_addr));

    // Connect to server
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    printf("Connected to server!\n");

    // Send message to server
    send(sock, buffer, strlen(buffer), 0);

    // Receive response
    recv(sock, buffer, sizeof(buffer), 0);
    printf("Received from server: %s\n", buffer);

    // Cleanup
    close(sock);

    return 0;
}
