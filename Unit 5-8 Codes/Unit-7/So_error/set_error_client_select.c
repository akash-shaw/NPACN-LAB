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

    // 1. Set socket to NON-BLOCKING
    int flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(3380);
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // 2. Initiate connection
    retval = connect(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));

    if (retval == -1) {
        if (errno == EINPROGRESS) {
            printf("Connection in progress...\n");

            fd_set writefds;
            struct timeval tv;
            FD_ZERO(&writefds);
            FD_SET(sockfd, &writefds);

            tv.tv_sec = 5; // 5 second timeout
            tv.tv_usec = 0;

            // 3. Monitor for writability
            retval = select(sockfd + 1, NULL, &writefds, NULL, &tv);

            if (retval > 0) {
                if (FD_ISSET(sockfd, &writefds)) {
                    int error = 0;
                    socklen_t len = sizeof(error);
                    
                    // 4. Use SO_ERROR to check if connection actually succeeded
                    if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len) < 0) {
                        perror("getsockopt failed");
                        close(sockfd);
                        exit(1);
                    }

                    if (error == 0) {
                        printf("Successfully connected!\n");
                    } else {
                        printf("Connection failed: %s\n", strerror(error));
                        close(sockfd);
                        exit(1);
                    }
                }
            } else if (retval == 0) {
                printf("Connection timeout.\n");
                close(sockfd);
                exit(1);
            } else {
                perror("select error");
                close(sockfd);
                exit(1);
            }
        } else {
            perror("Connect error");
            close(sockfd);
            exit(1);
        }
    }

    // Set back to blocking mode for simple communication
    fcntl(sockfd, F_SETFL, flags);

    printf("Enter message to server: ");
    fgets(buff, MAXSIZE, stdin);
    send(sockfd, buff, strlen(buff), 0);

    memset(buff, 0, MAXSIZE);
    recv(sockfd, buff, sizeof(buff), 0);
    printf("Server replied: %s\n", buff);

    close(sockfd);
    return 0;
}