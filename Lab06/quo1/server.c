#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8081

int count_ones(char *data) {
    int count = 0;
    for (int i = 0; i < strlen(data); i++) {
        if (data[i] == '1') {
            count++;
        }
    }
    return count;
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    int parity_choice;

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    
    // Listen
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("\n--- PARITY CHECK SERVER (RECEIVER) ---\n");
    printf("Waiting for connections...\n");

    while(1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        // 1. Read Parity Choice
        read(new_socket, &parity_choice, sizeof(int));
        
        // 2. Read Data Frame
        int valread = read(new_socket, buffer, 1024);
        buffer[valread] = '\0'; // Null terminate

        printf("\nReceived Frame: %s\n", buffer);
        printf("Parity Mode: %s\n", (parity_choice == 1) ? "Even" : "Odd");

        // 3. Verify
        int rx_ones = count_ones(buffer);
        char result[1024];

        if (parity_choice == 1) { // Even
            if (rx_ones % 2 == 0) {
                sprintf(result, "ACCEPTED (Total 1s: %d is Even)", rx_ones);
                printf("Result: %s\n", result);
            } else {
                sprintf(result, "ERROR DETECTED (Total 1s: %d is Odd)", rx_ones);
                printf("Result: %s\n", result);
            }
        } else { // Odd
            if (rx_ones % 2 != 0) {
                sprintf(result, "ACCEPTED (Total 1s: %d is Odd)", rx_ones);
                printf("Result: %s\n", result);
            } else {
                sprintf(result, "ERROR DETECTED (Total 1s: %d is Even)", rx_ones);
                printf("Result: %s\n", result);
            }
        }

        // 4. Send Response
        send(new_socket, result, strlen(result), 0);
        
        close(new_socket);
        // Break after one client for this lab example, or continue
        // Using exit(0) to just stop it nicely for testing, or assume iterative
        printf("Closing connection.\n");
    }
    return 0;
}
