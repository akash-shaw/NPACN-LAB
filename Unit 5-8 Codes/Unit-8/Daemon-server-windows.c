#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define LOG_FILE "daemon_log.txt"

void daemonize() {
    FreeConsole();  // Detach from the terminal
    FILE *log = fopen(LOG_FILE, "a");
    if (log) {
        fprintf(log, "Daemon process started\n");
        fclose(log);
    }
}

void handle_client(SOCKET client_socket) {
    char buffer[1024] = {0};
    recv(client_socket, buffer, sizeof(buffer), 0);
    printf("Client: %s\n", buffer);
    send(client_socket, "Hello from daemon server", 25, 0);
    closesocket(client_socket);
}

int main() {
    WSADATA wsa;
    SOCKET server_fd, client_socket;
    struct sockaddr_in server, client;
    int addrlen = sizeof(client);

    // Initialize Winsock
    WSAStartup(MAKEWORD(2, 2), &wsa);

    // Daemonize the process
    daemonize();

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr*)&server, sizeof(server));
    listen(server_fd, 5);

    while (1) {
        client_socket = accept(server_fd, (struct sockaddr*)&client, &addrlen);
        if (client_socket != INVALID_SOCKET) {
            handle_client(client_socket);
        }
    }

    closesocket(server_fd);
    WSACleanup();
    return 0;
}
