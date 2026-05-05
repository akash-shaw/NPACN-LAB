#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080

int main() {
    WSADATA wsaData;
    SOCKET serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    int addrSize, recvSize;
    char recvBuffer[1024], sendBuffer[1024];

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed. Exiting...\n");
        return 1;
    }

    // Create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        printf("Socket creation failed. Exiting...\n");
        WSACleanup();
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    // Bind socket
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Bind failed. Exiting...\n");
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // Listen for incoming connections
    if (listen(serverSocket, 3) == SOCKET_ERROR) {
        printf("Listen failed. Exiting...\n");
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    printf("Server is listening on port %d...\n", PORT);

    // Accept incoming connection
    addrSize = sizeof(clientAddr);
    clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrSize);
    if (clientSocket == INVALID_SOCKET) {
        printf("Accept failed. Exiting...\n");
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    printf("Client connected.\n");

    // Full-duplex communication: send and receive simultaneously
    while (1) {
        // Receive message from client
        recvSize = recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
        if (recvSize == SOCKET_ERROR) {
            printf("Receive failed. Exiting...\n");
            break;
        }
        recvBuffer[recvSize] = '\0';  // Null-terminate the received string
        printf("Received from client: %s\n", recvBuffer);

        // Send response to client
        printf("Enter message to send to client: \n");
        fgets(sendBuffer, sizeof(sendBuffer), stdin);
        send(clientSocket, sendBuffer, strlen(sendBuffer), 0);
    }

    // Close sockets
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
