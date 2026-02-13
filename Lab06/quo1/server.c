#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#define MAXSIZE 1024

int count_ones(char *data) {
    int count = 0;
    for (int i = 0; i < strlen(data); i++) {
        if (data[i] == '1') {
            count++;
        }
    }
    return count;
}

int main() {
    int sockfd, newsockfd, retval;
    socklen_t actuallen;
    struct sockaddr_in serveraddr, clientaddr;
    char buff[MAXSIZE];
    int parity_choice;
    int recedbytes, sentbytes;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("\nSocket creation error");
        exit(0);
    }
    
    // Allow reusing address
    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htons(INADDR_ANY);
    serveraddr.sin_port = htons(8081);

    retval = bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    if (retval == -1) {
        printf("Binding error");
        close(sockfd);
        exit(0);
    }
    
    retval = listen(sockfd, 1);
    if (retval == -1) {
        printf("Listen error");
        close(sockfd);
        exit(0);
    }

    printf("\n--- PARITY CHECK SERVER (RECEIVER) ---\n");
    printf("Waiting for connections...\n");

    while(1) {
        actuallen = sizeof(clientaddr);
        newsockfd = accept(sockfd, (struct sockaddr *)&clientaddr, &actuallen);
        if (newsockfd == -1) {
            printf("Accept error");
            close(sockfd);
            exit(0);
        }

        // 1. Read Parity Choice
        recedbytes = recv(newsockfd, &parity_choice, sizeof(int), 0);
        if (recedbytes <= 0) {
            close(newsockfd);
            continue;
        }
        
        // 2. Read Data Frame
        recedbytes = recv(newsockfd, buff, MAXSIZE, 0);
        if (recedbytes <= 0) {
            close(newsockfd);
            continue;
        }
        buff[recedbytes] = '\0'; // Null terminate

        printf("\nReceived Frame: %s\n", buff);
        printf("Parity Mode: %s\n", (parity_choice == 1) ? "Even" : "Odd");

        // 3. Verify
        int rx_ones = count_ones(buff);
        char result[MAXSIZE];

        if (parity_choice == 1) { // Even
            if (rx_ones % 2 == 0) {
                sprintf(result, "ACCEPTED (Total 1s: %d is Even)", rx_ones);
                printf("Result: %s\n", result);
            } else {
                sprintf(result, "ERROR DETECTED (Total 1s: %d is Odd)", rx_ones);
                printf("Result: %s\n", result);
            }
        } else { // Odd
            if (rx_ones % 2 != 0) {
                sprintf(result, "ACCEPTED (Total 1s: %d is Odd)", rx_ones);
                printf("Result: %s\n", result);
            } else {
                sprintf(result, "ERROR DETECTED (Total 1s: %d is Even)", rx_ones);
                printf("Result: %s\n", result);
            }
        }

        // 4. Send Response
        sentbytes = send(newsockfd, result, strlen(result), 0);
        
        close(newsockfd);
        printf("Closing connection.\n");
    }
    close(sockfd);
    return 0;
}
