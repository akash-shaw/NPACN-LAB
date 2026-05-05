#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsa;
    SOCKET client_socket;
    struct sockaddr_in server, client;
    int client_len;
    char message[1024];
    char buffer[1024];

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        perror("WSAStartup failed");
        return 1;
    }

    // Create a socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        perror("Socket creation failed");
        return 1;
    }

    // Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(12346);

    // Connect
    if (connect(client_socket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        perror("Connect failed");
        return 1;
    }

    printf("Connected to server.\n");

    // Get client's IP address and port number
    client_len = sizeof(client);
    if (getsockname(client_socket, (struct sockaddr*)&client, &client_len) == 0) {
        printf("Client IP Address: %s\n", inet_ntoa(client.sin_addr));
        printf("Client Port Number: %d\n", ntohs(client.sin_port));
    } else {
        perror("getsockname failed");
    }

    // Send data to server
    strcpy(message, "Hello, server!");
    send(client_socket, message, strlen(message), 0);

    // Receive response from server
    int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytes_received == SOCKET_ERROR) {
        perror("Receive failed");
        return 1;
    }

    buffer[bytes_received] = '\0';
    printf("Received from server: %s\n", buffer);

    closesocket(client_socket);
    WSACleanup();
    return 0;
}
