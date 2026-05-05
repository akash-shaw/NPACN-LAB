#define _WIN32_WINNT 0x0600  // Enables Windows Vista+ features like getaddrinfo()
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#define SERVER_IP "::1"  // IPv4 server address (converted to IPv4-mapped IPv6)
#define SERVER_PORT 8080

int main() {
    WSADATA wsaData;
    SOCKET clientSocket;
    struct sockaddr_in6 serverAddr;
    char buffer[1024] = "Hello from IPv6 Client!";
    struct addrinfo hints, *result;
    int disable_v6only = 0;

    // Initialize Winsock
   if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
    printf("WSAStartup failed: %d\n", WSAGetLastError());
    return 1;
}


    // Create an IPv6 socket
    clientSocket = socket(AF_INET6, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        perror("Socket creation failed");
        WSACleanup();
        return 1;
    }

    // Allow IPv6 socket to connect to IPv4-mapped addresses
    #ifdef IPV6_V6ONLY  // Ensure it is defined before using it
    setsockopt(clientSocket, IPPROTO_IPV6, IPV6_V6ONLY, (char*)&disable_v6only, sizeof(disable_v6only));
    #endif

    // Configure server address (IPv6)
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin6_family = AF_INET6;
    serverAddr.sin6_port = htons(SERVER_PORT);

    // Convert IPv4 address to IPv4-mapped IPv6 address (::ffff:<IPv4>)
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET6;  // Use IPv6 family
    hints.ai_socktype = SOCK_STREAM;

    #ifdef AI_V4MAPPED  // Ensure AI_V4MAPPED is available
    hints.ai_flags = 0;
    #endif

    // Use getaddrinfo to resolve the address and port
    if (getaddrinfo(SERVER_IP, NULL, &hints, &result) != 0) {
        perror("getaddrinfo failed");
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    // Copy the resolved IPv6 address to the serverAddr structure
    memcpy(&serverAddr.sin6_addr, &((struct sockaddr_in6 *)result->ai_addr)->sin6_addr, sizeof(struct in6_addr));
    freeaddrinfo(result);

    // Connect to server
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Connection failed");
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    printf("Connected to server!\n");

    // Send message to server
    send(clientSocket, buffer, strlen(buffer), 0);

    // Receive response
    recv(clientSocket, buffer, sizeof(buffer), 0);
    printf("Received from server: %s\n", buffer);

    // Cleanup
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
