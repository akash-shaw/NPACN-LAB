/*
 * Lab No. 6: Error Detection using CRC (Cyclic Redundancy Check)
 * Implements CRC-12, CRC-16, and CRC-CCITT
 */
#include <stdio.h>
#include <string.h>

// Standard Polynomials (Binary Representations)
// CRC-12: x^12 + x^11 + x^3 + x^2 + x + 1
#define POLY_CRC12 "1100000001111" 

// CRC-16: x^16 + x^15 + x^2 + 1
#define POLY_CRC16 "11000000000000101"

// CRC-CCITT: x^16 + x^12 + x^5 + 1
#define POLY_CCITT "10001000000100001"

char data[100], checksum[50], generator[50];
int data_len, gen_len;

// Function to perform XOR operation
void xor_operation(char *dividend, char *divisor) {
    for (int i = 0; i < gen_len; i++) {
        if (dividend[i] == divisor[i])
            dividend[i] = '0';
        else
            dividend[i] = '1';
    }
}

// Function to calculate CRC
void crc_computation(char *input_data, char *output_rem) {
    char temp[100];
    char current_dividend[100];
    
    strcpy(temp, input_data);
    
    // Initialize current dividend with first N bits (N = gen_len)
    strncpy(current_dividend, temp, gen_len);
    current_dividend[gen_len] = '\0';
    
    int i = gen_len;
    while (i <= strlen(temp)) {
        // If leading bit is 1, perform XOR with generator
        if (current_dividend[0] == '1') {
            xor_operation(current_dividend, generator);
        }
        // If leading bit is 0, XOR with 0s (effectively just shift, implemented below)
        
        // Shift bits left: drop MSB, pull in next bit from input
        for (int j = 0; j < gen_len - 1; j++) {
            current_dividend[j] = current_dividend[j+1];
        }
        
        // Append next bit from original data
        if (i < strlen(temp)) {
            current_dividend[gen_len - 1] = temp[i];
        }
        current_dividend[gen_len] = '\0'; // Null terminate to treat as string if needed internally, 
                                          // though the loop bounds handle logical termination.
        i++;
    }
    
    // The 'current_dividend' now holds the remainder (CRC value)
    // We only care about the first (gen_len - 1) bits for the checksum
    strncpy(output_rem, current_dividend, gen_len - 1);
    output_rem[gen_len - 1] = '\0';
}

int main() {
    char padded_data[100], transmitted_data[100], received_data[100];
    int choice;

    printf("\n--- CRC Error Detection ---\n");
    printf("Enter Data (Binary String, e.g., 100100): ");
    scanf("%s", data);
    data_len = strlen(data);

    printf("\nSelect CRC Polynomial:\n");
    printf("1. CRC-12 (x^12 + x^11 + x^3 + x^2 + x + 1)\n");
    printf("2. CRC-16 (x^16 + x^15 + x^2 + 1)\n");
    printf("3. CRC-CCITT (x^16 + x^12 + x^5 + 1)\n");
    printf("Enter choice: ");
    scanf("%d", &choice);

    switch(choice) {
        case 1: strcpy(generator, POLY_CRC12); break;
        case 2: strcpy(generator, POLY_CRC16); break;
        case 3: strcpy(generator, POLY_CCITT); break;
        default: printf("Invalid choice. Exiting.\n"); return 0;
    }
    gen_len = strlen(generator);

    // --- SENDER SIDE ---
    
    // 1. Append (N-1) zeros to data
    strcpy(padded_data, data);
    for (int i = 0; i < gen_len - 1; i++) {
        strcat(padded_data, "0");
    }

    printf("\nGenerator Polynomial: %s", generator);
    printf("\nModified Data (Appended Zeros): %s", padded_data);

    // 2. Compute CRC (Remainder)
    crc_computation(padded_data, checksum);
    
    printf("\nCalculated Checksum (CRC): %s", checksum);

    // 3. Create Transmitted Data (Data + Checksum)
    strcpy(transmitted_data, data);
    strcat(transmitted_data, checksum);
    
    printf("\nTransmitted Data: %s\n", transmitted_data);

    // --- CHANNEL SIMULATION ---
    
    printf("\n--- TRANSMISSION CHANNEL ---\n");
    printf("Enter Received Data (simulate error by changing bits): ");
    scanf("%s", received_data);

    // --- RECEIVER SIDE ---
    
    printf("\n--- RECEIVER SIDE ---\n");
    
    // Perform division on received data
    char recv_checksum[50];
    crc_computation(received_data, recv_checksum);

    // Check if remainder is all zeros
    int error = 0;
    for (int i = 0; i < strlen(recv_checksum); i++) {
        if (recv_checksum[i] == '1') {
            error = 1;
            break;
        }
    }

    if (error == 0) {
        printf("Remainder: %s\n", recv_checksum);
        printf("Result: Data is Valid (No Error Detected).\n");
        
    } else {
        printf("Remainder: %s\n", recv_checksum);
        printf("Result: Data is CORRUPTED (Error Detected).\n");
        
    }

    return 0;
}