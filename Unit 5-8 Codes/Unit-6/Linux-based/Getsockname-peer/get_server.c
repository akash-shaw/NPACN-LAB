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
    socklen_t actuallen, addrlen;
    int recedbytes, sentbytes;
    struct sockaddr_in serveraddr, clientaddr, localaddr, peeraddr;
    char buff[MAXSIZE];
    pid_t childpid=-1;

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
        printf("Binding error code: %d\n", errno);
        close(sockfd);
        exit(0);
    }

    listen(sockfd, 5);
    printf("Server listening on port 3380...\n");

    while (1) {
        actuallen = sizeof(clientaddr);
        newsockfd = accept(sockfd, (struct sockaddr *)&clientaddr, &actuallen);

        if (newsockfd == -1) {
            continue;
        }

        // Create child process for concurrency
        if ((childpid = fork()) == 0) {
            close(sockfd); // Child doesn't need the listener

            // --- GETSOCKNAME: Get local info ---
            addrlen = sizeof(localaddr);
            getsockname(newsockfd, (struct sockaddr *)&localaddr, &addrlen);
            printf("\nChild %d: Local IP is %s, Local Port is %d\n", 
                    getpid(), inet_ntoa(localaddr.sin_addr), ntohs(localaddr.sin_port));

            // --- GETPEERNAME: Get client info ---
            addrlen = sizeof(peeraddr);
            getpeername(newsockfd, (struct sockaddr *)&peeraddr, &addrlen);
            printf("Child %d: Peer IP is %s, Peer Port is %d\n", 
                    getpid(), inet_ntoa(peeraddr.sin_addr), ntohs(peeraddr.sin_port));

            while (1) {
                memset(buff, '\0', sizeof(buff));
                recedbytes = recv(newsockfd, buff, sizeof(buff), 0);

                if (recedbytes <= 0 || strncmp(buff, "quit", 4) == 0) {
                    printf("Child %d: Client disconnected.\n", getpid());
                    break;
                }

                printf("Client to Child %d: %s", getpid(), buff);
                
                char msg[] = "Acknowledged by Server\n";
                send(newsockfd, msg, strlen(msg), 0);
            }
            close(newsockfd);
            exit(0); 
        }
        
        // Parent process
        close(newsockfd); 
    }
}