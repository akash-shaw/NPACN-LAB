#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define MAXSIZE 1024
#define PORT 8083

int main() {
    int sockfd, newsockfd, retval;
    socklen_t actuallen;
    struct sockaddr_in serveraddr, clientaddr;
    int code[30], total_len, r=0;
    int recedbytes, sentbytes;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("\nSocket creation error");
        exit(0);
    }
    
    int opt=1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htons(INADDR_ANY);
    serveraddr.sin_port = htons(PORT);

    retval = bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    if (retval == -1) {
        printf("Binding error");
        close(sockfd);
        exit(0);
    }

    retval = listen(sockfd, 3);
    if (retval == -1) {
        printf("Listen error");
        close(sockfd);
        exit(0);
    }

    printf("\n--- HAMMING CODE SERVER ---\nWaiting...\n");

    while(1) {
        actuallen = sizeof(clientaddr);
        newsockfd = accept(sockfd, (struct sockaddr *)&clientaddr, &actuallen);
        if (newsockfd == -1) {
            printf("Accept error");
            close(sockfd);
            exit(0);
        }

        // 1. Receive Metadata
        recedbytes = recv(newsockfd, &total_len, sizeof(int), 0);
        if(recedbytes <= 0) {
            close(newsockfd);
            continue;
        }
        
        // 2. Receive Code Array
        recedbytes = recv(newsockfd, code, sizeof(int)*30, 0);
        if(recedbytes <= 0) {
            close(newsockfd);
            continue;
        }

        printf("\nReceived Code of length %d: ", total_len);
        for(int i=1; i<=total_len; i++) printf("%d", code[i]);
        printf("\n");

        // 3. Determine r
        int r_calc = 0;
        while ((1 << r_calc) <= total_len) { 
             r_calc++; 
        }
        
        // 4. Calculate Syndrome
        int error_pos = 0;
        for (int i = 0; i < r_calc; i++) {
            int position = pow(2, i);
            int count = 0;
            
            for (int j = position; j <= total_len; j++) {
                if (((j >> i) & 1) == 1) {
                    if (code[j] == 1) count++;
                }
            }
            if (count % 2 != 0) {
                error_pos += position;
            }
        }

        // 5. Result
        char result[500];
        if (error_pos == 0) {
            sprintf(result, "No Error Detected.");
        } else {
            sprintf(result, "Error Detected at Position: %d\n", error_pos);
            
            // Correction
            if(error_pos <= total_len) {
                code[error_pos] = (code[error_pos] == 0) ? 1 : 0;
                char corrected_str[100] = "";
                for(int i=1; i<=total_len; i++) {
                    char bit[2]; sprintf(bit, "%d", code[i]);
                    strcat(corrected_str, bit);
                }
                char append[200];
                sprintf(append, "Corrected Code: %s", corrected_str);
                strcat(result, append);
            }
        }

        printf("Outcome: %s\n", result);
        sentbytes = send(newsockfd, result, strlen(result), 0);
        close(newsockfd);
    }
    close(sockfd);
    return 0;
}