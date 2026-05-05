#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <errno.h>

#define MAXSIZE 90

// Function to handle individual client communication
void *handle_client(void *arg) {
    int newsockfd = *((int *)arg);
    free(arg); // Free the allocated memory for the socket descriptor
    char buff[MAXSIZE];
    int recedbytes, sentbytes;

    while (1) {
        memset(buff, '\0', sizeof(buff));
        recedbytes = recv(newsockfd, buff, sizeof(buff), 0);

        if (recedbytes <= 0) {
            printf("Client disconnected or error.\n");
            break;
        }

        printf("Client: %s", buff);

        // Echo back or process logic
        strcat(buff, " (Server Ack)");
        sentbytes = send(newsockfd, buff, sizeof(buff), 0);

        if (sentbytes == -1) {
            break;
        }
    }

    close(newsockfd);
    return NULL;
}

void main() {
    int sockfd, newsockfd, retval;
    socklen_t actuallen;
    struct sockaddr_in serveraddr, clientaddr;

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

    retval = listen(sockfd, 5);
    if (retval == -1) {
        close(sockfd);
        exit(0);
    }

    printf("Server listening on port 3380...\n");

    while (1) {
        actuallen = sizeof(clientaddr);
        newsockfd = accept(sockfd, (struct sockaddr *)&clientaddr, &actuallen);

        if (newsockfd != -1) {
            printf("Connection accepted from %s\n", inet_ntoa(clientaddr.sin_addr));
            
            pthread_t tid;
            int *pclient = malloc(sizeof(int));
            *pclient = newsockfd;
            
            // Create a new thread for each client
            pthread_create(&tid, NULL, handle_client, pclient);
            pthread_detach(tid); // Allow thread to clean up automatically
        }
    }
    close(sockfd);
}