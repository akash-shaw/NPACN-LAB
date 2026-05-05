#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_IP "127.0.0.1"
#define PORT 8080

int main() {
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server;
    char buffer[1024];

    WSAStartup(MAKEWORD(2, 2), &wsa);

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(SERVER_IP);
    server.sin_port = htons(PORT);

    // Connect to server
    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) == 0) {
        send(sock, "Hello from client", 17, 0);
        recv(sock, buffer, sizeof(buffer), 0);
        printf("Server: %s\n", buffer);
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}
