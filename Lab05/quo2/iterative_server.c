/*
 * TCP Iterative Server
 * Checks for Anagrams and logs client details with timestamp.
 * Handling: One client at a time (Iterative).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8888
#define BUFFER_SIZE 1024

// Helper function to sort a string
void sort_string(char *str) {
    int n = strlen(str);
    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            if (tolower(str[i]) > tolower(str[j])) {
                char temp = str[i];
                str[i] = str[j];
                str[j] = temp;
            }
        }
    }
}

// Function to check anagrams
int check_anagram(char *str1, char *str2) {
    // If lengths differ, they can't be anagrams
    if (strlen(str1) != strlen(str2)) return 0;

    char s1[BUFFER_SIZE], s2[BUFFER_SIZE];
    strcpy(s1, str1);
    strcpy(s2, str2);

    sort_string(s1);
    sort_string(s2);

    return (strcmp(s1, s2) == 0);
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address, cli_addr;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    char str1[BUFFER_SIZE], str2[BUFFER_SIZE];
    time_t rawtime;
    struct tm *timeinfo;

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

    printf("Iterative Server listening on port %d...\n", PORT);

    // Iterative Loop: Handle one client, then loop back for the next
    while (1) {
        int clilen = sizeof(cli_addr);
        
        // 4. Accept (Blocks until a client connects)
        if ((new_socket = accept(server_fd, (struct sockaddr *)&cli_addr, (socklen_t*)&clilen)) < 0) {
            perror("Accept failed");
            continue;
        }

        // 5. Display Date, Time, and Client Info
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        
        printf("\n--- Client Connected ---\n");
        printf("Client Address: %s\n", inet_ntoa(cli_addr.sin_addr));
        printf("Client Port: %d\n", ntohs(cli_addr.sin_port));
        printf("Time: %s", asctime(timeinfo));

        // 6. Communication Loop with Current Client
        while (1) {
            memset(buffer, 0, BUFFER_SIZE);
            
            // Read String 1
            int valread = read(new_socket, str1, BUFFER_SIZE);
            if (valread <= 0) break; // Client disconnected
            str1[valread] = '\0';
            
            // Read String 2
            valread = read(new_socket, str2, BUFFER_SIZE);
            str2[valread] = '\0';

            printf("Received strings: '%s' and '%s'\n", str1, str2);

            // Check Anagram
            if (check_anagram(str1, str2)) {
                strcpy(buffer, "Strings are Anagrams");
            } else {
                strcpy(buffer, "Strings are NOT Anagrams");
            }

            // Send Result
            send(new_socket, buffer, strlen(buffer), 0);
        }

        printf("Client disconnected.\n");
        close(new_socket);
        // Loop continues to Accept next client
    }
    
    close(server_fd);
    return 0;
}