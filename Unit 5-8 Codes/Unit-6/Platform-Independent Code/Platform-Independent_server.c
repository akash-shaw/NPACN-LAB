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

    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    char buffer[1024];
    int addr_len, recv_len;

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        perror("Socket creation failed");
        cleanup_sockets();
        exit(EXIT_FAILURE);
    }

    // Set up server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

	int opt = 1;
	if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
    	perror("setsockopt failed");
   	 cleanup_sockets();
    	exit(EXIT_FAILURE);
}

    // Bind the socket
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        fprintf(stderr, "Bind failed with error: %d\n", WSAGetLastError());  // Windows-specific
  	perror("Bind failed");
        cleanup_sockets();
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(server_socket, 1) == SOCKET_ERROR) {
        perror("Listen failed");
        cleanup_sockets();
        exit(EXIT_FAILURE);
    }
    printf("Server listening on port %d...\n", PORT);

    // Accept a client connection
    addr_len = sizeof(client_addr);
    client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_len);
    if (client_socket == INVALID_SOCKET) {
        perror("Accept failed");
        cleanup_sockets();
        exit(EXIT_FAILURE);
    }
    printf("Client connected.\n");

    // Receive data from the client
    recv_len = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    if (recv_len > 0) {
        buffer[recv_len] = '\0';
        printf("Received: %s\n", buffer);

        // Send a response
        send(client_socket, "Hello, Client!", 15, 0);
    }

    // Cleanup
#ifdef _WIN32
    closesocket(client_socket);
    closesocket(server_socket);
#else
    close(client_socket);
    close(server_socket);
#endif

    cleanup_sockets();
    return 0;
}
