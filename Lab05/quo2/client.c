/*
 * TCP Client
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8888
#define BUFFER_SIZE 1024

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char str1[BUFFER_SIZE], str2[BUFFER_SIZE];
    char buffer[BUFFER_SIZE] = {0};

    // 1. Create Socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // 2. Connect
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }
    
    printf("Connected to Server.\n");

    while (1) {
        printf("\n--- Anagram Checker ---\n");
        printf("Enter first string (or 'exit' to quit): ");
        scanf("%s", str1);
        
        if (strcmp(str1, "exit") == 0) break;

        printf("Enter second string: ");
        scanf("%s", str2);

        // 3. Send Strings
        send(sock, str1, strlen(str1), 0);
        usleep(1000); // Small delay to ensure separate packets/reads
        send(sock, str2, strlen(str2), 0);

        // 4. Receive Result
        memset(buffer, 0, BUFFER_SIZE);
        int valread = read(sock, buffer, BUFFER_SIZE);
        if (valread > 0) {
            printf("Server Response: %s\n", buffer);
        } else {
            printf("Server disconnected.\n");
            break;
        }
    }

    close(sock);
    return 0;
}