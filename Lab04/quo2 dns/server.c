/*
 * TCP DNS Server
 * Reference: Lab No. 4, Exercise 1
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Database Structure
struct DNS_Record {
    char hostname[50];
    char ip[20];
};

// Initialize some dummy data
struct DNS_Record database[] = {
    {"www.google.com", "142.250.183.68"},
    {"www.facebook.com", "157.240.22.35"},
    {"www.youtube.com", "142.250.183.78"},
    {"www.manipal.edu", "20.207.237.166"},
    {"www.yahoo.com", "98.137.11.163"}
};

int db_size = sizeof(database) / sizeof(database[0]);

// Function to search for IP
char* search_dns(char *host) {
    for (int i = 0; i < db_size; i++) {
        if (strcmp(database[i].hostname, host) == 0) {
            return database[i].ip;
        }
    }
    return "Host not found in database";
}

int main() {
    int sockfd, newsockfd;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;
    char buffer[BUFFER_SIZE];
    int n;

    // 1. Create Socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    // 2. Bind
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
        exit(1);
    }

    // 3. Listen
    listen(sockfd, 5);
    printf("DNS Server is listening on port %d...\n", PORT);

    clilen = sizeof(cli_addr);

    // 4. Accept Connection
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0) {
        perror("ERROR on accept");
        exit(1);
    }

    while (1) {
        bzero(buffer, BUFFER_SIZE);
        
        // 5. Read Hostname from Client
        n = read(newsockfd, buffer, BUFFER_SIZE);
        if (n <= 0) break;

        // Remove newline character if present (from fgets/input)
        buffer[strcspn(buffer, "\n")] = 0;

        printf("Client requested IP for: %s\n", buffer);

        // 6. Search Database
        if (strcmp(buffer, "exit") == 0) {
            break;
        }

        char *result = search_dns(buffer);

        // 7. Send Result to Client
        write(newsockfd, result, strlen(result));
    }

    close(newsockfd);
    close(sockfd);
    return 0;
}