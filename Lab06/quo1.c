/*
 * Lab No. 6: Error Detection using Parity Bit
 * Simulates Sender and Receiver logic for Odd/Even Parity.
 */
#include <stdio.h>
#include <string.h>

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
    char data[100], received_data[100];
    int parity_choice, ones, parity_bit;
    
    // --- SENDER SIDE ---
    printf("\n--- SENDER SIDE ---\n");
    printf("Enter Binary Data (e.g., 1011001): ");
    scanf("%s", data);
    
    printf("Select Parity Type:\n");
    printf("1. Even Parity\n");
    printf("2. Odd Parity\n");
    printf("Enter choice: ");
    scanf("%d", &parity_choice);
    
    // Calculate Parity Bit
    ones = count_ones(data);
    
    if (parity_choice == 1) { // Even Parity
        // If 1s are odd, add 1 to make it even. Else 0.
        if (ones % 2 != 0) parity_bit = 1;
        else parity_bit = 0;
    } else { // Odd Parity
        // If 1s are even, add 1 to make it odd. Else 0.
        if (ones % 2 == 0) parity_bit = 1;
        else parity_bit = 0;
    }
    
    // Append parity bit to data
    char transmitted_data[100];
    strcpy(transmitted_data, data);
    int len = strlen(data);
    transmitted_data[len] = parity_bit + '0'; // Convert int to char
    transmitted_data[len + 1] = '\0';
    
    printf("\nData to Send: %s\n", data);
    printf("Parity Bit Added: %d\n", parity_bit);
    printf("Transmitted Frame: %s\n", transmitted_data);
    
    // --- CHANNEL SIMULATION ---
    printf("\n--- TRANSMISSION CHANNEL ---\n");
    printf("Enter the data received (simulate error by changing bits): ");
    scanf("%s", received_data);
    
    // --- RECEIVER SIDE ---
    printf("\n--- RECEIVER SIDE ---\n");
    printf("Received Frame: %s\n", received_data);
    
    // Count 1s in the received frame (data + parity)
    int rx_ones = count_ones(received_data);
    
    if (parity_choice == 1) { // Even Parity Logic
        if (rx_ones % 2 == 0) {
            printf("Result: No Error Detected (Total 1s is Even).\n");
            printf("Accepted Data.\n");
        } else {
            printf("Result: Error Detected! (Total 1s is Odd).\n");
            printf("Rejecting Data.\n");
        }
    } else { // Odd Parity Logic
        if (rx_ones % 2 != 0) {
            printf("Result: No Error Detected (Total 1s is Odd).\n");
            printf("Accepted Data.\n");
        } else {
            printf("Result: Error Detected! (Total 1s is Even).\n");
            printf("Rejecting Data.\n");
        }
    }
    
    return 0;
}