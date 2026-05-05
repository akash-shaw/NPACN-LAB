#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>


#pragma comment(lib, "ws2_32.lib")  // Link Winsock library

#define SERVER_IP "127.0.0.1" // Server's IPv4 address
#define SERVER_PORT 8080

int main() {
    WSADATA wsaData;
    SOCKET sock;
    struct sockaddr_in sa;
    int sa_len = sizeof(sa);
    char ipString[] = "127.0.0.1";
    struct sockaddr_in serverAddr;
    char buffer[1024] = "Hello from IPv4 client!";

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed\n");
        return 1;
    }

    // Create IPv4 socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        printf("Socket creation failed: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Configure server address
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
  //  inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);
WSAStringToAddressA(ipString, AF_INET, NULL, (struct sockaddr*)&sa, &sa_len);
serverAddr.sin_addr = sa.sin_addr;

    // Connect to server
    if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        printf("Connection failed: %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    printf("Connected to server!\n");

    // Send message to server
    send(sock, buffer, strlen(buffer), 0);
    
    // Receive response
    recv(sock, buffer, sizeof(buffer), 0);
    printf("Received from server: %s\n", buffer);

    // Cleanup
    closesocket(sock);
    WSACleanup();

    return 0;
}
