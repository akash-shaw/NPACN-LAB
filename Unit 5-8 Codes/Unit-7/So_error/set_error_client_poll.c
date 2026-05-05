#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>

#define MAXSIZE 90

int main() {
    int sockfd, retval;
    struct sockaddr_in serveraddr;
    char buff[MAXSIZE];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation error");
        exit(1);
    }

    // 1. Set to NON-BLOCKING mode
    int flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(3380);
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // 2. Attempt connection
    printf("Connecting to server...\n");
    retval = connect(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));

    if (retval == -1 && errno == EINPROGRESS) {
        struct pollfd pfd;
        pfd.fd = sockfd;
        pfd.events = POLLOUT; // Connection success makes the socket writable

        // 3. Wait for poll (5 second timeout)
        int poll_ret = poll(&pfd, 1, 5000);

        if (poll_ret > 0) {
            int error = 0;
            socklen_t len = sizeof(error);
            
            // 4. Check SO_ERROR to see if connection succeeded
            if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len) < 0) {
                perror("getsockopt failed");
                close(sockfd);
                exit(1);
            }

            if (error == 0) {
                printf("Successfully connected using poll()!\n");
            } else {
                fprintf(stderr, "Connection failed: %s\n", strerror(error));
                close(sockfd);
                exit(1);
            }
        } else {
            printf("Connection timed out or poll error.\n");
            close(sockfd);
            exit(1);
        }
    } else if (retval == -1) {
        perror("Immediate connect error");
        close(sockfd);
        exit(1);
    }

    // 5. Set back to BLOCKING mode for simple I/O
    fcntl(sockfd, F_SETFL, flags);

    printf("Enter message: ");
    fgets(buff, MAXSIZE, stdin);
    send(sockfd, buff, strlen(buff), 0);

    memset(buff, 0, MAXSIZE);
    recv(sockfd, buff, sizeof(buff), 0);
    printf("Server replied: %s\n", buff);

    close(sockfd);
    return 0;
}