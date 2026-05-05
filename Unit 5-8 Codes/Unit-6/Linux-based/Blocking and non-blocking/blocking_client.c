#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    // 1. Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8085);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    // 2. Connect (This BLOCKS until server is ready)
    printf("Client: Attempting to connect...\n");
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection Failed");
        return -1;
    }
    printf("Connected to server!\n");

    // 3. Send data
    send(sock, "Hello from Blocking Client", 26, 0);

    // 4. Receive data (This BLOCKS until server sends something)
    read(sock, buffer, 1024);
    printf("Server replied: %s\n", buffer);

    close(sock);
    return 0;
}