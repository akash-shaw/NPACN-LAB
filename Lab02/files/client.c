#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX 1024

int main() {
    int sockfd;
    struct sockaddr_in serveraddr;
    char filename[MAX], buffer[MAX], str1[MAX], str2[MAX];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(3388);
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0) {
        perror("Connect failed"); exit(1);
    }

    printf("Enter filename: ");
    scanf("%s", filename);
    send(sockfd, filename, MAX, 0);

    recv(sockfd, buffer, MAX, 0);
    if (strcmp(buffer, "NOT_FOUND") == 0) {
        printf("Server: File not present. Terminating.\n");
        close(sockfd); return 0;
    }

    int choice;
    while (1) {
        printf("----MENU----");
        printf("\n1. Search\n");
        printf("\n2. Replace\n");
        printf("\n3. Reorder\n");
        printf("\n4. Exit\n");
        printf("\n\n Choice:");
        // printf("\n1. Search\n2. Replace\n3. Reorder\n4. Exit\nChoice: ");
        scanf("%d", &choice);
        send(sockfd, &choice, sizeof(int), 0);

        if (choice == 4) break;

        if (choice == 1) {
            printf("Enter string to search: ");
            scanf("%s", str1);
            send(sockfd, str1, MAX, 0);
        } else if (choice == 2) {
            printf("Enter string to replace: "); scanf("%s", str1);
            printf("Enter new string: "); scanf("%s", str2);
            send(sockfd, str1, MAX, 0);
            send(sockfd, str2, MAX, 0);
        }

        memset(buffer, 0, MAX);
        recv(sockfd, buffer, MAX, 0);
        printf("Server Response: %s\n", buffer);
    }

    close(sockfd);
    return 0;
}