/*
 * TCP File Server
 * Returns file content and statistics (size, lines, spaces, etc.)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080
#define BUFFER_SIZE 4096

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    char filename[100];
    FILE *fp;

    // 1. Create Socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // 2. Bind
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // 3. Listen
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    
    printf("TCP Server waiting for client...\n");

    // 4. Accept
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        memset(filename, 0, 100);

        // Read Filename
        int valread = read(new_socket, filename, 100);
        if (valread <= 0) break;

        // Check for stop condition
        if (strcmp(filename, "stop") == 0) {
            printf("Client sent 'stop'. Terminating session.\n");
            break;
        }

        printf("Client requested: %s\n", filename);

        fp = fopen(filename, "r");
        if (fp == NULL) {
            char *msg = "File not present";
            send(new_socket, msg, strlen(msg), 0);
        } else {
            // Statistics counters
            int alphabets = 0, digits = 0, spaces = 0, lines = 0, others = 0, size = 0;
            char ch;
            char file_content[BUFFER_SIZE - 500]; // Reserve space for stats header
            int i = 0;

            // Read file char by char
            while ((ch = fgetc(fp)) != EOF) {
                size++;
                if (i < sizeof(file_content) - 1) {
                    file_content[i++] = ch;
                }
                
                if (isalpha(ch)) alphabets++;
                else if (isdigit(ch)) digits++;
                else if (ch == ' ') spaces++;
                else if (ch == '\n') lines++;
                else others++;
            }
            file_content[i] = '\0';
            fclose(fp);

            if (size > 0 && file_content[size-1] != '\n') {
                lines++; // Count last line if no newline at EOF
            }

            // Format Response
            // Using a large buffer to send everything in one go
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

            send(new_socket, buffer, strlen(buffer), 0);
        }
    }

    close(new_socket);
    close(server_fd);
    return 0;
}