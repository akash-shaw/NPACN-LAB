#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serveraddr = {
        .sin_family = AF_INET,
        .sin_port = htons(3380),
        .sin_addr.s_addr = inet_addr("127.0.0.1")
    };

    if (connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == 0) {
        char *msg = "Priority Alert: System Check";
        send(sockfd, msg, strlen(msg), 0);
        printf("Message sent to Daemon.\n");
    } else {
        perror("Connection failed");
    }

    close(sockfd);
    return 0;
}