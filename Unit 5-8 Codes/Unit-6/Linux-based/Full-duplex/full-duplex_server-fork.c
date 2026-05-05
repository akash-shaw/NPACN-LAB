#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int server_fd, client_sock;
    struct sockaddr_in address;
    char buffer[1024];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8081);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 1);
    
    printf("Server listening on 8080. Waiting for client...\n");
    client_sock = accept(server_fd, NULL, NULL);
    printf("Client connected! (Full-Duplex Mode On)\n");

    if (fork() == 0) {
        // CHILD PROCESS: Always Listening (Recv)
        while (1) {
            memset(buffer, 0, 1024);
            if (recv(client_sock, buffer, 1024, 0) <= 0) break;
            printf("\n[FROM CLIENT]: %s", buffer);
            printf("Server Command > "); fflush(stdout);
        }
    } else {
        // PARENT PROCESS: Always Sending (Keyboard Input)
        while (1) {
            printf("Server Command > ");
            fgets(buffer, 1024, stdin);
            send(client_sock, buffer, strlen(buffer), 0);
        }
    }

    close(client_sock);
    return 0;
}