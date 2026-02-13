#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#define MAXSIZE 1024

// Poly Definitions
#define POLY_CRC12 "1100000001111" 
#define POLY_CRC16 "11000000000000101"
#define POLY_CCITT "10001000000100001"

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
    
    // Initialize current dividend
    strncpy(current_dividend, temp, gen_len);
    current_dividend[gen_len] = '\0';
    
    int i = gen_len;
    while (i <= strlen(temp)) {
        if (current_dividend[0] == '1') {
            xor_operation(current_dividend, generator, gen_len);
        }
        
        // Shift
        for (int j = 0; j < gen_len - 1; j++) {
            current_dividend[j] = current_dividend[j+1];
        }
        
        if (i < strlen(temp)) {
            current_dividend[gen_len - 1] = temp[i];
        }
        current_dividend[gen_len] = '\0';
        i++;
    }
    
    strncpy(output_rem, current_dividend, gen_len - 1);
    output_rem[gen_len - 1] = '\0';
}

int main() {
    int sockfd, retval;
    struct sockaddr_in serveraddr;
    char buff[MAXSIZE];
    char data[100], generator[50], padded_data[200], checksum[50];
    char transmitted_data[200], error_data[200];
    int choice;
    int recedbytes, sentbytes;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("\nSocket Creation Error");
        return 0;
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(8082);
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    retval = connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    if (retval == -1) {
        printf("\nConnection Failed \n");
        close(sockfd);
        return 0;
    }

    // --- INPUT ---
    printf("\n--- CRC CLIENT (SENDER) ---\n");
    printf("Enter Data (Binary String): ");
    scanf("%s", data);

    printf("Select CRC Polynomial:\n");
    printf("1. CRC-12\n2. CRC-16\n3. CRC-CCITT\nChoice: ");
    scanf("%d", &choice);

    switch(choice) {
        case 1: strcpy(generator, POLY_CRC12); break;
        case 2: strcpy(generator, POLY_CRC16); break;
        case 3: strcpy(generator, POLY_CCITT); break;
        default: strcpy(generator, POLY_CRC12); break;
    }

    // --- CALCULATION ---
    int gen_len = strlen(generator);
    strcpy(padded_data, data);
    for(int i=0; i<gen_len-1; i++) strcat(padded_data, "0");

    crc_computation(padded_data, checksum, generator);
    
    strcpy(transmitted_data, data);
    strcat(transmitted_data, checksum);

    printf("\nGenerated Checksum: %s\n", checksum);
    printf("Final Codeword: %s\n", transmitted_data);

    // --- ERROR INJECTION ---
    printf("Enter data to send (simulate error): ");
    scanf("%s", error_data);

    // --- SENDING ---
    // 1. Send Generator First
    sentbytes = send(sockfd, generator, strlen(generator), 0);
    if (sentbytes == -1) {
        printf("Error sending generator");
        close(sockfd);
        return 0;
    }

    // Sleep briefly to ensure separation or use fixed size
    usleep(100000); 
    
    // 2. Send Frame
    sentbytes = send(sockfd, error_data, strlen(error_data), 0);
    if (sentbytes == -1) {
        printf("Error sending frame");
        close(sockfd);
        return 0;
    }

    // --- RECEIVE RESPONSE ---
    recedbytes = recv(sockfd, buff, MAXSIZE, 0);
    if (recedbytes == -1) {
        printf("Error receiving response");
        close(sockfd);
        return 0;
    }
    buff[recedbytes] = '\0';
    printf("\nServer Response: %s\n", buff);

    close(sockfd);
    return 0;
}
