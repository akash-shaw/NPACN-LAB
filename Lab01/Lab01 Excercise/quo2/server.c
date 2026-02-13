#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT 8080
#define MAXLINE 1024

// Function to check if a string is a palindrome
int checkPalindrome(char *str) {
    int len = strlen(str);
    int start = 0;
    int end = len - 1;
    while (end > start) {
        if (str[start++] != str[end--]) {
            return 0; // Not a palindrome
        }
    }
    return 1; // Is a palindrome
}

int main() {
    int sockfd;
    char buffer[MAXLINE];
    struct sockaddr_in servaddr, cliaddr;
    
    // 1. Create Socket File Descriptor (UDP)
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));
    
    // 2. Fill Server Information
    servaddr.sin_family = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);
    
    // 3. Bind the socket with the server address
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
    
    printf("UDP Server is listening on port %d...\n", PORT);
    
    int n;
    socklen_t len;
    
    while (1) {
        len = sizeof(cliaddr);
        
        // 4. Receive Message from Client
        n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL, (struct sockaddr *)&cliaddr, &len);
        buffer[n] = '\0';
        
        // Check for termination condition
        if (strcmp(buffer, "Halt") == 0) {
            printf("Client sent 'Halt'. Server terminating.\n");
            break;
        }
        
        printf("Client Sent: %s\n", buffer);
        
        // 5. Process Data
        // A. Length
        int length = strlen(buffer);
        
        // B. Palindrome Check
        int isPal = checkPalindrome(buffer);
        
        // C. Vowel Count
        int v_a = 0, v_e = 0, v_i = 0, v_o = 0, v_u = 0;
        for (int i = 0; i < length; i++) {
            char ch = tolower(buffer[i]);
            if (ch == 'a') v_a++;
            else if (ch == 'e') v_e++;
            else if (ch == 'i') v_i++;
            else if (ch == 'o') v_o++;
            else if (ch == 'u') v_u++;
        }
        
        // 6. Format Response
        char response[MAXLINE];
        sprintf(response, 
                "\n--- Results ---\n"
                "Length: %d\n"
                "Palindrome: %s\n"
                "Vowels Count -> a: %d, e: %d, i: %d, o: %d, u: %d\n", 
                length, (isPal ? "Yes" : "No"), v_a, v_e, v_i, v_o, v_u);
        
        // 7. Send Response to Client
        sendto(sockfd, (const char *)response, strlen(response), MSG_CONFIRM, (const struct sockaddr *)&cliaddr, len);
    }
    
    close(sockfd);
    return 0;
}