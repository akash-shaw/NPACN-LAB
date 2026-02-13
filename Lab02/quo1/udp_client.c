#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8888
#define BUFFER_SIZE 2048

int main() {
    int sockfd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in servaddr;
    char filename[100];

    // 1. Create Socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    socklen_t len = sizeof(servaddr);

    // Send Filename
    printf("Enter filename: ");
    scanf("%s", filename);
    
    char msg[200];
    sprintf(msg, "FILE %s", filename);
    sendto(sockfd, msg, strlen(msg), MSG_CONFIRM, (const struct sockaddr *)&servaddr, len);

    // Receive Status
    int n = recvfrom(sockfd, (char *)buffer, BUFFER_SIZE, MSG_WAITALL, (struct sockaddr *)&servaddr, &len);
    buffer[n] = '\0';

    if (strcmp(buffer, "File not present") == 0) {
        printf("%s\n", buffer);
        close(sockfd);
        exit(0);
    }
    printf("Server: %s\n", buffer);

    int choice;
    while (1) {
        printf("\n--- MENU ---\n");
        printf("1. Search\n2. Replace\n3. Reorder\n4. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        memset(msg, 0, sizeof(msg));

        if (choice == 1) {
            char str[100];
            printf("Enter string to search: ");
            scanf("%s", str);
            sprintf(msg, "1 %s", str);
        } else if (choice == 2) {
            char str1[50], str2[50];
            printf("Enter string to find: ");
            scanf("%s", str1);
            printf("Enter replace string: ");
            scanf("%s", str2);
            sprintf(msg, "2 %s %s", str1, str2);
        } else if (choice == 3) {
            strcpy(msg, "3");
        } else if (choice == 4) {
            strcpy(msg, "4");
            sendto(sockfd, msg, strlen(msg), MSG_CONFIRM, (const struct sockaddr *)&servaddr, len);
            break;
        }

        sendto(sockfd, msg, strlen(msg), MSG_CONFIRM, (const struct sockaddr *)&servaddr, len);

        n = recvfrom(sockfd, (char *)buffer, BUFFER_SIZE, MSG_WAITALL, (struct sockaddr *)&servaddr, &len);
        buffer[n] = '\0';
        printf("Server Response: %s\n", buffer);
    }

    close(sockfd);
    return 0;
}