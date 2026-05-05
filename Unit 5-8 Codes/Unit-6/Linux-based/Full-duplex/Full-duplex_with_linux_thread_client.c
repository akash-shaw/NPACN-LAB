#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8080

int clientSocket;

// Thread function for receiving data
void* receiveHandler(void* arg) {
    char buffer[1024];
    int bytesReceived;

    while (1) {
        bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) {
            printf("Server disconnected or error occurred.\n");
            break;
        }
        buffer[bytesReceived] = '\0';
        printf("Server: %s\n", buffer);
    }
    return NULL;
}

// Thread function for sending data
void* sendHandler(void* arg) {
    char buffer[1024];

    while (1) {
        printf("Enter message to send to server (type 'exit' to quit): ");
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
    struct sockaddr_in serverAddr;

    // Create socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);

    // Connect to server
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Connection failed");
        close(clientSocket);
        exit(EXIT_FAILURE);
    }
    printf("Connected to server at %s:%d\n", SERVER_IP, PORT);

    // Create threads for sending and receiving
    pthread_t recvThread, sendThread;
    pthread_create(&recvThread, NULL, receiveHandler, NULL);
    pthread_create(&sendThread, NULL, sendHandler, NULL);

    // Wait for threads to finish
    pthread_join(sendThread, NULL);

    // Cleanup
    close(clientSocket);

    return 0;
}
