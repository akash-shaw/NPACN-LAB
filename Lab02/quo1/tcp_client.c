#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 2048

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    char filename[100];
    char input[100];

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

    // Send Filename
    printf("Enter filename: ");
    scanf("%s", filename);
    send(sock, filename, strlen(filename), 0);

    // Check File Status
    read(sock, buffer, BUFFER_SIZE);
    if (strcmp(buffer, "File not present") == 0) {
        printf("%s\n", buffer);
        close(sock);
        return 0;
    }
    printf("Server: %s\n", buffer);

    int choice;
    while (1) {
        printf("\n--- MENU ---\n");
        printf("1. Search\n2. Replace\n3. Reorder\n4. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        memset(buffer, 0, BUFFER_SIZE);
        
        if (choice == 1) {
            char str[100];
            printf("Enter string to search: ");
            scanf("%s", str);
            sprintf(buffer, "1 %s", str);
            send(sock, buffer, strlen(buffer), 0);

        } else if (choice == 2) {
            char str1[100], str2[100];
            printf("Enter string to find: ");
            scanf("%s", str1);
            printf("Enter replace string: ");
            scanf("%s", str2);
            sprintf(buffer, "2 %s %s", str1, str2);
            send(sock, buffer, strlen(buffer), 0);

        } else if (choice == 3) {
            strcpy(buffer, "3");
            send(sock, buffer, strlen(buffer), 0);

        } else if (choice == 4) {
            strcpy(buffer, "4");
            send(sock, buffer, strlen(buffer), 0);
            break;
        }

        // Receive Result
        memset(buffer, 0, BUFFER_SIZE);
        read(sock, buffer, BUFFER_SIZE);
        printf("Server Response: %s\n", buffer);
    }

    close(sock);
    return 0;
}