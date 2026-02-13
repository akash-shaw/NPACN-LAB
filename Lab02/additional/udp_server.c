/*
 * UDP File Server
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8888
#define BUFFER_SIZE 4096

int main() {
    int sockfd;
    char buffer[BUFFER_SIZE];
    char filename[100];
    struct sockaddr_in servaddr, cliaddr;
    FILE *fp;

    // 1. Create Socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // 2. Bind
    servaddr.sin_family = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    printf("UDP Server listening on port %d...\n", PORT);
    
    socklen_t len;
    int n;

    while (1) {
        len = sizeof(cliaddr);
        memset(buffer, 0, BUFFER_SIZE);
        memset(filename, 0, 100);

        // Receive Filename
        n = recvfrom(sockfd, (char *)filename, 100, MSG_WAITALL, (struct sockaddr *)&cliaddr, &len);
        filename[n] = '\0';

        if (strcmp(filename, "stop") == 0) {
            printf("Client sent stop signal. (Server continues listening for others)\n");
            continue;
        }

        printf("Request for file: %s\n", filename);

        fp = fopen(filename, "r");
        if (fp == NULL) {
            char *msg = "File not present";
            sendto(sockfd, (const char *)msg, strlen(msg), MSG_CONFIRM, (const struct sockaddr *)&cliaddr, len);
        } else {
            int alphabets = 0, digits = 0, spaces = 0, lines = 0, others = 0, size = 0;
            char ch;
            char file_content[BUFFER_SIZE - 500]; 
            int i = 0;

            while ((ch = fgetc(fp)) != EOF) {
                size++;
                if (i < sizeof(file_content) - 1) file_content[i++] = ch;
                
                if (isalpha(ch)) alphabets++;
                else if (isdigit(ch)) digits++;
                else if (ch == ' ') spaces++;
                else if (ch == '\n') lines++;
                else others++;
            }
            file_content[i] = '\0';
            fclose(fp);
            
             if (size > 0 && file_content[size-1] != '\n') lines++;

            sprintf(buffer, 
                "--- File Statistics ---\n"
                "Size: %d bytes\n"
                "Alphabets: %d\n"
                "Digits: %d\n"
                "Spaces: %d\n"
                "Lines: %d\n"
                "Others: %d\n"
                "--- File Content ---\n%s",
                size, alphabets, digits, spaces, lines, others, file_content);

            sendto(sockfd, (const char *)buffer, strlen(buffer), MSG_CONFIRM, (const struct sockaddr *)&cliaddr, len);
        }
    }
    return 0;
}