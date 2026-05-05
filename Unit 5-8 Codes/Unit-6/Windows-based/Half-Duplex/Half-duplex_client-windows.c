#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_IP "127.0.0.1"
#define PORT 8080

int main() {
    WSADATA wsaData;
    SOCKET clientSocket;
    struct sockaddr_in serverAddr;
    char recvBuffer[1024], sendBuffer[1024];
    int recvSize;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed. Exiting...\n");
        return 1;
    }

    // Create socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        printf("Socket creation failed. Exiting...\n");
        WSACleanup();
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Connect to server
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Connection failed. Exiting...\n");
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    printf("Connected to server at %s:%d\n", SERVER_IP, PORT);

    // Full-duplex communication: send and receive simultaneously
    while (1) {
        // Send message to server
        printf("Enter message to send to server: \n");
        fgets(sendBuffer, sizeof(sendBuffer), stdin);
        send(clientSocket, sendBuffer, strlen(sendBuffer), 0);

        // Receive message from server
        recvSize = recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
        if (recvSize == SOCKET_ERROR) {
            printf("Receive failed. Exiting...\n");
            break;
        }
        recvBuffer[recvSize] = '\0';  // Null-terminate the received string
        printf("Received from server: %s\n", recvBuffer);
    }

    // Close socket
    closesocket(clientSocket);
    WSACleanup();
    return 0;
}
