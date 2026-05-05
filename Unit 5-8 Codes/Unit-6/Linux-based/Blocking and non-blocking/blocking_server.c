#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    char buffer[1024] = {0};

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8085);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 3);

    printf("Blocking Server: Waiting for a connection...\n");
    // This call blocks here until a client connects
    new_socket = accept(server_fd, NULL, NULL); 
    printf("Connection accepted!\n");

    read(new_socket, buffer, 1024);
    printf("Client said: %s\n", buffer);

    close(new_socket);
    close(server_fd);
    return 0;
}