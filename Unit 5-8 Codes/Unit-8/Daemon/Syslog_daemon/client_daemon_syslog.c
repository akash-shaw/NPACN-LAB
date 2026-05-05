#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#define MAXSIZE 90

int main() {
    int sockfd;
    struct sockaddr_in serveraddr;
    char buff[MAXSIZE] = "Hello Daemon!";

    // 1. Create Socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(3380);
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // 2. Connect to Server
    printf("Connecting to daemon on port 3380...\n");
    if (connect(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) == 0) {
        
        // 3. Send Message (including the null terminator for safety)
        if (send(sockfd, buff, strlen(buff) + 1, 0) < 0) {
            perror("Send failed");
        } else {
            printf("Message sent: %s\n", buff);
        }

        // 4. Receive Response
        memset(buff, 0, MAXSIZE); // Clear buffer
        int bytes_received = recv(sockfd, buff, MAXSIZE - 1, 0);
        
        if (bytes_received > 0) {
            printf("Response from Daemon: %s\n", buff);
        } else if (bytes_received == 0) {
            printf("Server closed the connection.\n");
        } else {
            perror("Recv failed");
        }

    } else {
        printf("Connection failed (Error %d): %s\n", errno, strerror(errno));
        printf("Is the daemon running? Check 'ps -axl | grep server_daemon'\n");
    }

    close(sockfd);
    return 0;
}