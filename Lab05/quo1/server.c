/*
 * Lab 5: Concurrent Server (Multiple Clients)
 * Appends strings from clients to a file.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <fcntl.h>

#define PORT 8888
#define BUFFER_SIZE 1024
#define FILENAME "result.txt"

int main() {
    int sockfd, newsockfd;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;
    pid_t childpid;
    int client_count = 0;
    char buffer[BUFFER_SIZE];
    FILE *fp;

    // 1. Initialize File with "Manipal"
    fp = fopen(FILENAME, "w");
    if (fp == NULL) {
        perror("Error opening file");
        exit(1);
    }
    fprintf(fp, "Manipal ");
    fclose(fp);
    printf("Server started. 'Manipal' written to file.\n");

    // 2. Create Socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    // 3. Bind
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Bind failed");
        exit(1);
    }

    // 4. Listen
    listen(sockfd, 5);
    printf("Concurrent Server listening on port %d...\n", PORT);

    clilen = sizeof(cli_addr);

    while (1) {
        // 5. Accept Connection
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        if (newsockfd < 0) {
            perror("Accept failed");
            exit(1);
        }

        client_count++;
        printf("Client %d connected from Port %d\n", client_count, ntohs(cli_addr.sin_port));

        // 6. Check Client Limit
        if (client_count > 2) {
            printf("Max clients exceeded. Sending termination signal.\n");
            strcpy(buffer, "terminate session");
            write(newsockfd, buffer, strlen(buffer));
            close(newsockfd);
            close(sockfd);
            printf("Server Terminating...\n");
            exit(0); // Terminate Server
        }

        // 7. Fork to handle client concurrently
        if ((childpid = fork()) == 0) {
            // --- Child Process ---
            close(sockfd); // Child doesn't need listener

            memset(buffer, 0, BUFFER_SIZE);
            read(newsockfd, buffer, BUFFER_SIZE);

            // Append to File
            // Note: Using 'a' mode (append)
            fp = fopen(FILENAME, "a");
            if (fp != NULL) {
                fprintf(fp, "%s ", buffer);
                fclose(fp);
            }

            // Read File Content to Display
            fp = fopen(FILENAME, "r");
            char file_content[BUFFER_SIZE];
            if (fp != NULL) {
                fgets(file_content, BUFFER_SIZE, fp);
                fclose(fp);
            }

            printf("\n--- Update ---\n");
            printf("Client Socket: %s:%d\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
            printf("Received: %s\n", buffer);
            printf("Current File Content: %s\n", file_content);
            printf("--------------\n");

            close(newsockfd);
            exit(0); // Child exits
        }
        
        // --- Parent Process ---
        close(newsockfd); // Parent closes connected socket
    }

    return 0;
}