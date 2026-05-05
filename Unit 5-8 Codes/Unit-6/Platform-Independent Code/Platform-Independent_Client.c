#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Platform-specific includes
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#define SOCKET int
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#endif

#define SERVER_IP 127.0.0.1
#define PORT 8081

// Platform-specific initialization and cleanup
void initialize_sockets() {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        fprintf(stderr, "WSAStartup failed.\n");
        exit(EXIT_FAILURE);
    }
#endif
}

void cleanup_sockets() {
#ifdef _WIN32
    WSACleanup();
#endif
}

int main() {
    initialize_sockets();

    SOCKET client_socket;
    struct sockaddr_in server_addr;
    char buffer[1024];

    // Create socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == INVALID_SOCKET) {
        perror("Socket creation failed");
        cleanup_sockets();
        exit(EXIT_FAILURE);
    }

    // Set up server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

/*
    #ifdef _WIN32
    // Use inet_pton on Windows
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Invalid address or address not supported");
        cleanup_sockets();
        exit(EXIT_FAILURE);
    }
#else
    // Use inet_pton on Linux
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Invalid address or address not supported");
        cleanup_sockets();
        exit(EXIT_FAILURE);
    }
#endif
*/

    // Connect to the server
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        perror("Connection failed");
        cleanup_sockets();
        exit(EXIT_FAILURE);
    }
    printf("Connected to server.\n");

    // Send data to the server
    send(client_socket, "Hello, Server!", 15, 0);

    // Receive response from the server
    int recv_len = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    if (recv_len > 0) {
        buffer[recv_len] = '\0';
        printf("Server response: %s\n", buffer);
    }

    // Cleanup
#ifdef _WIN32
    closesocket(client_socket);
#else
    close(client_socket);
#endif

    cleanup_sockets();
    return 0;
}
