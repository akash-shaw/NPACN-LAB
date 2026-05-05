#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib") // Link Winsock library

#define MAXSOCKADDR 128

int sockfd_to_family(SOCKET sockfd) {
    union {
        struct sockaddr sa;
        char data[MAXSOCKADDR];
    } un;
    int len = MAXSOCKADDR;

    // Use getsockname with Winsock's sockaddr
    if (getsockname(sockfd, (struct sockaddr*) &un.sa, &len) == SOCKET_ERROR) {
        return -1; // Error occurred
    }
    return un.sa.sa_family; // Return the address family (e.g., AF_INET)
}

int main() {
    WSADATA wsaData;
    SOCKET sockfd;
    struct sockaddr_in serverAddr;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed.\n");
        return -1;
    }

    // Create a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == INVALID_SOCKET) {
        printf("Socket creation failed.\n");
        WSACleanup();
        return -1;
    }

    // Bind the socket to an address (required to test getsockname)
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(8080);

    if (bind(sockfd, (struct sockaddr*) &serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Bind failed.\n");
        closesocket(sockfd);
        WSACleanup();
        return -1;
    }

    // Test sockfd_to_family
    int family = sockfd_to_family(sockfd);
    if (family == -1) {
        printf("Failed to get socket family.\n");
    } else if (family == AF_INET) {
        printf("Socket family: AF_INET\n");
    } else if (family == AF_INET6) {
        printf("Socket family: AF_INET6\n");
    } else {
        printf("Socket family: Unknown (%d)\n", family);
    }

    // Cleanup
    closesocket(sockfd);
    WSACleanup();

    return 0;
}
