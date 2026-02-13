#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8081

// Function to count number of 1s in a binary string
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
    int sock = 0;
    struct sockaddr_in serv_addr;
    char data[100];
    char transmitted_data[100];
    char error_data[100];
    char buffer[1024] = {0};
    int parity_choice, ones, parity_bit;

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

    // --- SENDER LOGIC ---
    printf("\n--- PARITY CHECK CLIENT (SENDER) ---\n");
    printf("Enter Binary Data (e.g., 1011001): ");
    scanf("%s", data);
    
    printf("Select Parity Type:\n");
    printf("1. Even Parity\n");
    printf("2. Odd Parity\n");
    printf("Enter choice: ");
    scanf("%d", &parity_choice);

    // Calculate Parity
    ones = count_ones(data);
    if (parity_choice == 1) { // Even Parity
        parity_bit = (ones % 2 != 0) ? 1 : 0;
    } else { // Odd Parity
        parity_bit = (ones % 2 == 0) ? 1 : 0;
    }

    // specific send for parity choice first
    write(sock, &parity_choice, sizeof(int));

    // Construct Frame
    strcpy(transmitted_data, data);
    int len = strlen(data);
    transmitted_data[len] = parity_bit + '0'; 
    transmitted_data[len+1] = '\0';

    printf("\nOriginal Calculated Frame: %s\n", transmitted_data);
    
    // Error Injection
    printf("Enter the data to actually send (change bits to simulate noise): ");
    scanf("%s", error_data);

    // Send
    send(sock, error_data, strlen(error_data), 0);
    printf("Data sent to server.\n");

    // Receive Result
    read(sock, buffer, 1024);
    printf("\nServer Response: %s\n", buffer);

    close(sock);
    return 0;
}
