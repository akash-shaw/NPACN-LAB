#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>

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
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char generator[100] = {0};
    char received_frame[200] = {0};
    char remainder[100];

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed"); exit(EXIT_FAILURE);
    }
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed"); exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen"); exit(EXIT_FAILURE);
    }

    printf("\n--- CRC SERVER (RECEIVER) ---\n");
    
    while(1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) exit(EXIT_FAILURE);

        // 1. Read Generator
        int val = read(new_socket, generator, 100);
        generator[val] = '\0';
        
        // 2. Read Frame
        val = read(new_socket, received_frame, 200);
        received_frame[val] = '\0';

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
        send(new_socket, result, strlen(result), 0);
        close(new_socket);
    }
    return 0;
}
