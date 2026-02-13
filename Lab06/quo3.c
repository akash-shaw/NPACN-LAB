/*
 * Lab No. 6: Hamming Code (Error Detection and Correction)
 * Standard Implementation: Even Parity
 */
#include <stdio.h>
#include <math.h>
#include <string.h>

int main() {
    int data[30], code[30], rec_code[30];
    int m, r = 0, i, j, k;
    
    // --- SENDER SIDE ---
    printf("\n--- SENDER SIDE ---\n");
    printf("Enter number of data bits: ");
    scanf("%d", &m);

    // 1. Input Data Bits
    printf("Enter the data bits (one by one, starting from D1): \n");
    for (i = 1; i <= m; i++) {
        scanf("%d", &data[i]);
    }

    // 2. Calculate number of redundancy bits (r)
    // Formula: 2^r >= m + r + 1
    while (pow(2, r) < (m + r + 1)) {
        r++;
    }
    printf("Number of redundancy bits (r) required: %d\n", r);
    printf("Total bits to transmit: %d\n", m + r);

    // 3. Construct the Code Word (Placeholders)
    // Parity bits go to positions 1, 2, 4, 8...
    // Data bits fill the rest
    j = 1; // Iterator for data bits
    k = 0; // Iterator for parity power (0->1, 1->2, 2->4...)
    
    for (i = 1; i <= m + r; i++) {
        // If i is a power of 2 (1, 2, 4...), it's a Parity bit
        if (i == pow(2, k)) {
            code[i] = 0; // Initialize parity bit to 0 for calculation
            k++;
        } else {
            code[i] = data[j];
            j++;
        }
    }

    // 4. Calculate Parity Bits
    // For each parity bit position (1, 2, 4...), check relevant bits
    for (i = 0; i < r; i++) {
        int position = pow(2, i);
        int count = 0;
        
        // Check bits that this parity bit covers
        // Logic: Start at 'position', check 'position' bits, skip 'position' bits
        for (j = position; j <= m + r; j++) {
            // Bitwise logic: if the position bit is set in binary representation of j
            if (((j >> i) & 1) == 1) {
                if (code[j] == 1) {
                    count++;
                }
            }
        }
        
        // If count is odd, parity bit should be 1 (for Even Parity)
        if (count % 2 != 0) {
            code[position] = 1;
        } else {
            code[position] = 0;
        }
    }

    // 5. Display Transmitted Code
    printf("\nTransmitted Code Word (Bit 1 to %d):\n", m + r);
    for (i = 1; i <= m + r; i++) {
        printf("%d ", code[i]);
    }
    printf("\n");

    // --- CHANNEL SIMULATION ---
    printf("\n--- TRANSMISSION CHANNEL ---\n");
    printf("Enter the received code word bits (introduce an error to test):\n");
    for (i = 1; i <= m + r; i++) {
        scanf("%d", &rec_code[i]);
    }

    // --- RECEIVER SIDE ---
    printf("\n--- RECEIVER SIDE ---\n");
    
    int error_pos = 0;
    
    // 6. Calculate Syndrome (Error Position)
    // Recalculate parity based on received data
    for (i = 0; i < r; i++) {
        int position = pow(2, i);
        int count = 0;
        
        for (j = position; j <= m + r; j++) {
             if (((j >> i) & 1) == 1) {
                if (rec_code[j] == 1) {
                    count++;
                }
            }
        }
        
        // If count is odd, there is an error in this parity check
        if (count % 2 != 0) {
            // Add the weight of this parity bit to the error position
            error_pos += position;
        }
    }

    // 7. Detection and Correction
    if (error_pos == 0) {
        printf("Result: No Error Detected. Data is correct.\n");
    } else {
        printf("Result: Error Detected at Position %d!\n", error_pos);
        
        printf("Original Received Data: ");
        for (i = 1; i <= m + r; i++) printf("%d ", rec_code[i]);
        printf("\n");

        // Correct the error (Flip the bit)
        if (rec_code[error_pos] == 0) 
            rec_code[error_pos] = 1;
        else 
            rec_code[error_pos] = 0;

        printf("Corrected Data:       ");
        for (i = 1; i <= m + r; i++) printf("%d ", rec_code[i]);
        printf("\n");
        
    }

    return 0;
}