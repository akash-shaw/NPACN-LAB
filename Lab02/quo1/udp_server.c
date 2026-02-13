#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

#define PORT 8888
#define BUFFER_SIZE 2048

// (Include the same helper functions: file_exists, search_string, replace_string, reorder_file here from the TCP code)
// For brevity, assuming they are copied here.
int file_exists(const char *filename) { return access(filename, F_OK) != -1; }
// ... [Insert helper functions here] ...
// Simplified dummy helpers for compilation context if checking structure:
int search_string(char *f, char *s) { return 0; } // Placeholder, use logic from TCP
int replace_string(char *f, char *s1, char *s2) { return 0; } // Placeholder
void reorder_file(char *f) {} // Placeholder

int main() {
    int sockfd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len;
    char current_filename[100] = {0};

    // 1. Create Socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    // 2. Bind
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    printf("UDP Server listening...\n");
    len = sizeof(cliaddr);

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int n = recvfrom(sockfd, (char *)buffer, BUFFER_SIZE, MSG_WAITALL, (struct sockaddr *)&cliaddr, &len);
        buffer[n] = '\0';

        char response[BUFFER_SIZE] = {0};
        
        // Protocol: "FILE filename" or "OP code data"
        if (strncmp(buffer, "FILE ", 5) == 0) {
            strcpy(current_filename, buffer + 5);
            printf("Client sent file: %s\n", current_filename);
            
            if (file_exists(current_filename)) {
                strcpy(response, "OK");
            } else {
                strcpy(response, "File not present");
                // Note: In UDP we just reply. If we must terminate per lab manual, we exit.
                // But usually servers stay alive. We will follow client logic to terminate.
            }
            sendto(sockfd, response, strlen(response), MSG_CONFIRM, (const struct sockaddr *)&cliaddr, len);
            if (strcmp(response, "File not present") == 0) {
                printf("File not found. Resetting/Exiting logic.\n");
                // exit(0); // Uncomment if server must die strictly
            }
        } 
        else {
            // It's a menu operation
            int choice = buffer[0] - '0';
            
            if (choice == 1) { // Search
                int count = search_string(current_filename, buffer + 2);
                if (count > 0) sprintf(response, "String occurred %d times", count);
                else strcpy(response, "String not found");
            } 
            else if (choice == 2) { // Replace
                char str1[50], str2[50];
                sscanf(buffer + 2, "%s %s", str1, str2);
                if (replace_string(current_filename, str1, str2)) strcpy(response, "String replaced");
                else strcpy(response, "String not found");
            } 
            else if (choice == 3) { // Reorder
                reorder_file(current_filename);
                strcpy(response, "File reordered successfully");
            } 
            else if (choice == 4) { // Exit
                 printf("Client exited.\n");
                 continue;
            }
            sendto(sockfd, response, strlen(response), MSG_CONFIRM, (const struct sockaddr *)&cliaddr, len);
        }
    }
    return 0;
}