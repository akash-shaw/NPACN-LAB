#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    char buffer[1024];

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8081);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connect failed");
        return -1;
    }
    printf("Connected to server! (Full-Duplex Mode On)\n");

    if (fork() == 0) {
        // CHILD PROCESS: Always Listening (Recv)
        while (1) {
            memset(buffer, 0, 1024);
            if (recv(sock, buffer, 1024, 0) <= 0) break;
            printf("\n[FROM SERVER]: %s", buffer);
            printf("Client Command > "); fflush(stdout);
        }
    } else {
        // PARENT PROCESS: Always Sending (Keyboard Input)
        while (1) {
            printf("Client Command > ");
            fgets(buffer, 1024, stdin);
            send(sock, buffer, strlen(buffer), 0);
        }
    }

    close(sock);
    return 0;
}