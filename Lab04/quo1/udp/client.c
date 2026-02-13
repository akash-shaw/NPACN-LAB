#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 9091
#define BUFFER_SIZE 1024

struct Request {
    int option;
    char data[100];
};

int main() {
    int sockfd;
    struct sockaddr_in serv_addr;
    struct Request req;
    char response[BUFFER_SIZE];
    socklen_t len;

    // 1. Create Socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    while (1) {
        printf("\n--- Student Info System (UDP) ---\n");
        printf("1. Registration Number\n");
        printf("2. Name of Student\n");
        printf("3. Subject Code\n");
        printf("4. Exit\n");
        printf("Enter Option: ");
        scanf("%d", &req.option);

        if (req.option == 4) {
            break;
        }

        printf("Enter Required Detail: ");
        scanf(" %[^\n]", req.data);

        // 2. Send Request
        sendto(sockfd, &req, sizeof(req), MSG_CONFIRM, (const struct sockaddr *)&serv_addr, sizeof(serv_addr));

        // 3. Receive Response
        memset(response, 0, BUFFER_SIZE);
        len = sizeof(serv_addr);
        int n = recvfrom(sockfd, response, BUFFER_SIZE, MSG_WAITALL, (struct sockaddr *)&serv_addr, &len);
        response[n] = '\0';

        printf("\n--- Server Response ---\n%s\n", response);
    }

    close(sockfd);
    return 0;
}