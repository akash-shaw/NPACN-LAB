#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8082

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
    int sock = 0;
    struct sockaddr_in serv_addr;
    char data[100], generator[50], padded_data[200], checksum[50];
    char transmitted_data[200], error_data[200];
    char buffer[1024];
    int choice;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
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
    write(sock, generator, strlen(generator));
    // Sleep briefly to ensure separation or use fixed size
    usleep(100000); 
    
    // 2. Send Frame
    write(sock, error_data, strlen(error_data));

    // --- RECEIVE RESPONSE ---
    read(sock, buffer, 1024);
    printf("\nServer Response: %s\n", buffer);

    close(sock);
    return 0;
}
