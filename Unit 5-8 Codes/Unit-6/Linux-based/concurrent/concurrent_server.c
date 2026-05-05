#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#define MAXSIZE 90

void main() {
    int sockfd, newsockfd, retval;
    socklen_t actuallen;
    int recedbytes, sentbytes;
    struct sockaddr_in serveraddr, clientaddr;
    char buff[MAXSIZE];
    pid_t childpid;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("\nSocket creation error");
        exit(0);
    }

    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(3380);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    retval = bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    if (retval < 0) {
        printf("Binding error\n");
        close(sockfd);
        exit(0);
    }

    listen(sockfd, 5);
    printf("Server listening... (Fork model)\n");

    while (1) {
        actuallen = sizeof(clientaddr);
        newsockfd = accept(sockfd, (struct sockaddr *)&clientaddr, &actuallen);

        if (newsockfd < 0) {
            exit(1);
        }

        printf("Connection accepted from %s:%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

        // Create a child process
        if ((childpid = fork()) == 0) {
            // CHILD PROCESS
            close(sockfd); // Child doesn't need the listener socket

            while (1) {
                memset(buff, '\0', sizeof(buff));
                recedbytes = recv(newsockfd, buff, sizeof(buff), 0);

                if (recedbytes <= 0 || strncmp(buff, "quit", 4) == 0) {
                    printf("Client disconnected from process %d\n", getpid());
                    break;
                }

                printf("Process %d received: %s", getpid(), buff);
                
                // Echo logic
                strcat(buff, " - processed by child");
                send(newsockfd, buff, sizeof(buff), 0);
            }
            close(newsockfd);
            exit(0); // Terminate child process when done
        }

        // PARENT PROCESS
        close(newsockfd); // Parent doesn't need the specific client socket
    }
    close(sockfd);
}