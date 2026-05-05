#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int choice;
    int port;
    char msg[1024];

    printf("1. Echo Server\n2. Time Server\n3. Uppercase Server\n");
    printf("Enter choice: ");
    scanf("%d", &choice);
    getchar();

    if (choice == 1) port = 9001;
    else if (choice == 2) port = 9002;
    else port = 9003;

    int sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serv;
    serv.sin_family = AF_INET;
    serv.sin_port = htons(port);
    serv.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(sock, (struct sockaddr *)&serv, sizeof(serv));

    if (choice != 2) {
        printf("Enter message: ");
        fgets(msg, sizeof(msg), stdin);
        write(sock, msg, strlen(msg));
    }

    read(sock, msg, sizeof(msg));
    printf("Server Response: %s\n", msg);

    close(sock);
    return 0;
}
