#include <stdio.h>
#include <winsock2.h>
#include <windows.h> // For Windows threads

#pragma comment(lib, "ws2_32.lib")

#define SERVER_IP "127.0.0.1"
#define PORT 8080

SOCKET clientSocket;

// Thread function for receiving data
DWORD WINAPI receiveHandler(LPVOID arg) {
    char recvBuffer[1024];
    int recvSize;

    while (1) {
        recvSize = recv(clientSocket, recvBuffer, sizeof(recvBuffer) - 1, 0);
        if (recvSize <= 0) {
            printf("Server disconnected or error occurred.\n");
            break;
        }
        recvBuffer[recvSize] = '\0';
        printf("Server: %s\n", recvBuffer);
    }
    return 0;
}

// Thread function for sending data
DWORD WINAPI sendHandler(LPVOID arg) {
    char sendBuffer[1024];

    while (1) {
        printf("Enter message to send to server: ");
        fgets(sendBuffer, sizeof(sendBuffer), stdin);

        // Check for exit condition
        if (strncmp(sendBuffer, "exit", 4) == 0) {
            printf("Exiting send loop...\n");
            break;
        }

        send(clientSocket, sendBuffer, strlen(sendBuffer), 0);
    }
    return 0;
}

int main() {
    WSADATA wsaData;
    struct sockaddr_in serverAddr;

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

    // Create threads for send and receive
    HANDLE recvThread, sendThread;
    recvThread = CreateThread(NULL, 0, receiveHandler, NULL, 0, NULL);
    sendThread = CreateThread(NULL, 0, sendHandler, NULL, 0, NULL);

    // Wait for threads to finish
    WaitForSingleObject(sendThread, INFINITE);

    // Cleanup
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
