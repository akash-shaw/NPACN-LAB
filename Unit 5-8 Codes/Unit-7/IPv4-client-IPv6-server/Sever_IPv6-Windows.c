#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#ifndef IPV6_V6ONLY
#define IPV6_V6ONLY 27  // Windows uses 27 as the option value
#endif
#pragma comment(lib, "ws2_32.lib")  // Link Winsock library

#define PORT 8080

int main() {
    WSADATA wsaData;
    SOCKET serverSocket, clientSocket;
    struct sockaddr_in6 serverAddr, clientAddr;
    int addrLen = sizeof(clientAddr);
    char buffer[1024];

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed\n");
        return 1;
    }

    // Create IPv6 socket
    serverSocket = socket(AF_INET6, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        printf("Socket creation failed: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Allow IPv4 clients to connect via IPv4-mapped IPv6 addresses
int opt = 0;  // 0 means allow both IPv4 & IPv6
    
    if (setsockopt(serverSocket, IPPROTO_IPV6, IPV6_V6ONLY, (char*)&opt, sizeof(opt)) < 0) {
        printf("Failed to disable IPv6-only mode\n");
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // Configure server address
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin6_family = AF_INET6;
    serverAddr.sin6_port = htons(PORT);
    serverAddr.sin6_addr = in6addr_any; // Accept connections on all interfaces



    // Bind the socket
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Bind failed: %d\n", WSAGetLastError());
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // Listen for incoming connections
    if (listen(serverSocket, 5) == SOCKET_ERROR) {
        printf("Listen failed: %d\n", WSAGetLastError());
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    printf("Server listening on IPv6 port %d...\n", PORT);

    // Accept a client connection
    clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrLen);
    if (clientSocket == INVALID_SOCKET) {
        printf("Accept failed: %d\n", WSAGetLastError());
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    printf("Client connected!\n");

    // Receive message from client
    recv(clientSocket, buffer, sizeof(buffer), 0);
    printf("Received from client: %s\n", buffer);

    // Send response
    send(clientSocket, "Hello from IPv6 server!", 24, 0);

    // Cleanup
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
