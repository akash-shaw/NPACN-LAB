#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define PORT 8080

int clientSocket;

// Thread function for receiving data
void* receiveHandler(void* arg) {
    char buffer[1024];
    int bytesReceived;

    while (1) {
        bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) {
            printf("Client disconnected or error occurred.\n");
            break;
        }
        buffer[bytesReceived] = '\0';
        printf("Client: %s\n", buffer);
    }
    return NULL;
}

// Thread function for sending data
void* sendHandler(void* arg) {
    char buffer[1024];

    while (1) {
        printf("Enter message to send to client (type = "exit" to "quite"): ");
        fgets(buffer, sizeof(buffer), stdin);

        // Check for exit condition
        if (strncmp(buffer, "exit", 4) == 0) {
            printf("Exiting send loop...\n");
            break;
        }

        send(clientSocket, buffer, strlen(buffer), 0);
    }
    return NULL;
}

int main() {
    int serverSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrLen = sizeof(clientAddr);

    // Create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    // Bind socket
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Bind failed");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(serverSocket, 1) == -1) {
        perror("Listen failed");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d...\n", PORT);

    // Accept client connection
    clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrLen);
    if (clientSocket == -1) {
        perror("Accept failed");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }
    printf("Client connected.\n");

    // Create threads for sending and receiving
    pthread_t recvThread, sendThread;
    pthread_create(&recvThread, NULL, receiveHandler, NULL);
    pthread_create(&sendThread, NULL, sendHandler, NULL);

    // Wait for threads to finish
    pthread_join(sendThread, NULL);

    // Cleanup
    close(clientSocket);
    close(serverSocket);

    return 0;
}
