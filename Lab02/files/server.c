#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX 1024

void sort_file(char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return;
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    rewind(fp);
    
    char *buffer = malloc(size + 1);
    fread(buffer, 1, size, fp);
    buffer[size] = '\0';
    fclose(fp);

    // ASCII Sort
    for (int i = 0; i < size - 1; i++) {
        for (int j = i + 1; j < size; j++) {
            if (buffer[i] > buffer[j]) {
                char t = buffer[i]; buffer[i] = buffer[j]; buffer[j] = t;
            }
        }
    }

    fp = fopen(filename, "w");
    fputs(buffer, fp);
    fclose(fp);
    free(buffer);
}

int main() {
    int sockfd, newsockfd;
    struct sockaddr_in serveraddr, clientaddr;
    socklen_t len = sizeof(clientaddr);
    char filename[MAX], buffer[MAX], str1[MAX], str2[MAX];
    int opt = 1;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    // Allow port reuse (prevents "Address already in use" errors)
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(3388);
    serveraddr.sin_addr.s_addr = INADDR_ANY;

    if(bind(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0) {
        perror("Bind failed"); exit(1);
    }
    
    listen(sockfd, 1);
    printf("Server listening on port 3388...\n");

    newsockfd = accept(sockfd, (struct sockaddr*)&clientaddr, &len);

    // 1. Receive filename
    memset(filename, 0, MAX);
    recv(newsockfd, filename, MAX, 0);

    if (access(filename, F_OK) == -1) {
        send(newsockfd, "NOT_FOUND", MAX, 0);
        close(newsockfd); close(sockfd);
        return 0;
    }
    send(newsockfd, "FOUND", MAX, 0);

    // 2. Menu Loop
    int choice;
    while (recv(newsockfd, &choice, sizeof(int), 0) > 0) {
        if (choice == 4) break;

        if (choice == 1) { // Search
            recv(newsockfd, str1, MAX, 0);
            FILE *fp = fopen(filename, "r");
            int count = 0;
            while (fscanf(fp, "%s", buffer) != EOF) {
                if (strcmp(buffer, str1) == 0) count++;
            }
            fclose(fp);
            if (count > 0) sprintf(buffer, "Occurrences: %d", count);
            else strcpy(buffer, "String not found");
            send(newsockfd, buffer, MAX, 0);
        } 
        else if (choice == 2) { // Replace
            recv(newsockfd, str1, MAX, 0);
            recv(newsockfd, str2, MAX, 0);
            FILE *fp = fopen(filename, "r");
            FILE *temp = fopen("temp.txt", "w");
            int found = 0;
            while (fscanf(fp, "%s", buffer) != EOF) {
                if (strcmp(buffer, str1) == 0) {
                    fprintf(temp, "%s ", str2);
                    found = 1;
                } else fprintf(temp, "%s ", buffer);
            }
            fclose(fp); fclose(temp);
            remove(filename); rename("temp.txt", filename);
            send(newsockfd, found ? "String replaced" : "String not found", MAX, 0);
        }
        else if (choice == 3) { // Sort
            sort_file(filename);
            send(newsockfd, "File reordered by ASCII", MAX, 0);
        }
    }

    close(newsockfd); close(sockfd);
    return 0;
}