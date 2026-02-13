#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 9091
#define BUFFER_SIZE 1024

// Structure to receive data
struct Request {
    int option;
    char data[100];
};

int main() {
    int sockfd;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t len;
    struct Request req;
    char response[BUFFER_SIZE];

    // 1. Create Socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    // 2. Bind
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    if (bind(sockfd, (const struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Bind failed");
        exit(1);
    }

    printf("UDP Server listening on port %d...\n", PORT);

    while (1) {
        len = sizeof(cli_addr);
        memset(&req, 0, sizeof(req));

        // 3. Receive Packet
        recvfrom(sockfd, &req, sizeof(req), MSG_WAITALL, (struct sockaddr *)&cli_addr, &len);

        printf("Received Request: Option %d, Data: %s\n", req.option, req.data);

        // 4. Assign to Child Process
        if (fork() == 0) {
            // --- Child Process Logic ---
            memset(response, 0, BUFFER_SIZE);
            
            if (req.option == 1) {
                sprintf(response, "Name: Jane Doe\nAddress: 456 Elm St\nChild PID: %d", getpid());
            } else if (req.option == 2) {
                sprintf(response, "Dept: IT, Sem: 6, Sec: B\nCourses: IT201, IT202\nChild PID: %d", getpid());
            } else if (req.option == 3) {
                sprintf(response, "Subject: %s\nMarks Obtained: 92\nChild PID: %d", req.data, getpid());
            } else {
                sprintf(response, "Invalid Option");
            }

            // Send Response back to client
            sendto(sockfd, response, strlen(response), MSG_CONFIRM, (const struct sockaddr *)&cli_addr, len);
            
            exit(0); // Child terminates after handling request
        }
        // Parent continues loop to receive next packet
    }
    return 0;
}