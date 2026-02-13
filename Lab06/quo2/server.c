#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#define MAXSIZE 1024
#define PORT 8082

void xor_operation(char *dividend, char *divisor, int gen_len) {
    for (int i = 0; i < gen_len; i++) {
        if (dividend[i] == divisor[i])
            dividend[i] = '0';
        else
            dividend[i] = '1';
    }
}

void crc_computation(char *input_data, char *output_rem, char *generator) {
    char temp[200];
    char current_dividend[100];
    int gen_len = strlen(generator);
    
    strcpy(temp, input_data);
    strncpy(current_dividend, temp, gen_len);
    current_dividend[gen_len] = '\0';
    
    int i = gen_len;
    while (i <= strlen(temp)) {
        if (current_dividend[0] == '1') {
            xor_operation(current_dividend, generator, gen_len);
        }
        for (int j = 0; j < gen_len - 1; j++) {
            current_dividend[j] = current_dividend[j+1];
        }
        if (i < strlen(temp)) current_dividend[gen_len - 1] = temp[i];
        current_dividend[gen_len] = '\0';
        i++;
    }
    strncpy(output_rem, current_dividend, gen_len - 1);
    output_rem[gen_len - 1] = '\0';
}

int main() {
    int sockfd, newsockfd, retval;
    socklen_t actuallen;
    int recedbytes, sentbytes;
    struct sockaddr_in serveraddr, clientaddr;
    char buffer[MAXSIZE];
    char generator[100] = {0};
    char received_frame[200] = {0};
    char remainder[100];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("\nSocket creation error");
        exit(0);
    }
    int opt = 1;
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

    printf("\n--- CRC SERVER (RECEIVER) ---\n");
    
    while(1) {
        actuallen = sizeof(clientaddr);
        newsockfd = accept(sockfd, (struct sockaddr *)&clientaddr, &actuallen);
        if (newsockfd == -1) {
            printf("Accept error");
            exit(0);
        }

        // 1. Read Generator
        recedbytes = recv(newsockfd, generator, 100, 0);
        if (recedbytes <= 0) {
            close(newsockfd);
            continue;
        }
        generator[recedbytes] = '\0';
        
        // 2. Read Frame
        recedbytes = recv(newsockfd, received_frame, 200, 0);
        if (recedbytes <= 0) {
             // In case packet merged or missed, error handling simplistic here
        }
        received_frame[recedbytes] = '\0';

        printf("\nReceived Generator: %s\n", generator);
        printf("Received Frame: %s\n", received_frame);

        // 3. Verify
        crc_computation(received_frame, remainder, generator);
        
        printf("Calculated Remainder: %s\n", remainder);

        int error = 0;
        for(int k=0; k<strlen(remainder); k++) {
            if(remainder[k] == '1') error=1;
        }

        char result[200];
        if(error == 0) {
            sprintf(result, "ACCEPTED. Remainder is all zeros.");
        } else {
            sprintf(result, "REJECTED. Error detected (Non-zero remainder).");
        }
        
        printf("Result: %s\n", result);
        send(newsockfd, result, strlen(result), 0);
        close(newsockfd);
    }
    close(sockfd);
    return 0;
}
