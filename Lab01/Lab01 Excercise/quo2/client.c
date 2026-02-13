/*
 * UDP Client Program
 * Reference: Lab No. 1, Exercise 2
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT 8080
#define MAXLINE 1024

int main() {
    int sockfd;
    char buffer[MAXLINE];
    char message[MAXLINE];
    struct sockaddr_in servaddr;
    
    // 1. Create Socket File Descriptor (UDP)
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    memset(&servaddr, 0, sizeof(servaddr));
    
    // 2. Fill Server Information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Connect to localhost
    
    int n;
    socklen_t len;
    
    while (1) {
        printf("\nEnter a string (Enter 'Halt' to exit): ");
        scanf("%s", message);
        
        // 3. Send Message to Server
        sendto(sockfd, (const char *)message, strlen(message), MSG_CONFIRM, (const struct sockaddr *)&servaddr, sizeof(servaddr));
        
        // Check for termination condition locally to exit client loop
        if (strcmp(message, "Halt") == 0) {
            printf("Terminating client...\n");
            break;
        }
        
        // 4. Receive Response from Server
        len = sizeof(servaddr);
        n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL, (struct sockaddr *)&servaddr, &len);
        buffer[n] = '\0';
        
        printf("%s", buffer);
    }
    
    close(sockfd);
    return 0;
}