#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8083

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int code[30], total_len, r=0;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) exit(EXIT_FAILURE);
    
    int opt=1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 3);

    printf("\n--- HAMMING CODE SERVER ---\nWaiting...\n");

    while(1) {
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);

        // 1. Receive Metadata
        read(new_socket, &total_len, sizeof(int));
        
        // 2. Receive Code Array
        read(new_socket, code, sizeof(int)*30);

        printf("\nReceived Code of length %d: ", total_len);
        for(int i=1; i<=total_len; i++) printf("%d", code[i]);
        printf("\n");

        // 3. Determine r (reverse calc or just counting powers of 2 until > total_len)
        r = 0;
        while(pow(2, r) <= total_len) { // wait, if total_len is 7, r=3 (1,2,4). 2^3=8 > 7.
             // Actually, strictly: m+r=total_len.
             // We just need to know how many parity bits are checked.
             // Standard: check all powers of 2 <= total_len.
             if(pow(2, r) > total_len) break; // Optimization, but loop condition handles it
             r++;
        }
        // Actually, the loop just needs to iterate i from 0 while 2^i <= total_len
        // Let's stick to the Receiver Logic from original code which iterates based on r
        // The sender calculated r such that 2^r >= total + 1.
        // Let's recalculate r correctly.
        int r_calc = 0;
        while ((1 << r_calc) <= total_len) { 
             r_calc++; 
        }
        // Example: len=7 (bits 1..7). r should be 3 (1,2,4). 2^0=1, 2^1=2, 2^2=4.
        // loop: i=0 (1<=7), i=1 (2<=7), i=2 (4<=7), i=3 (8>7). So r_calc becomes 3. Correct.
        
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
        send(new_socket, result, strlen(result), 0);
        close(new_socket);
    }
}
