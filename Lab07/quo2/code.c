#include <stdio.h>

int main() {
    // Initial configuration based on the problem statement
    int max_bucket_size = 50;   // b = 50 KB
    int token_rate = 10;        // r = 10 KBps
    int tokens = 50;            // Bucket starts full
    
    int packet_size = 15;       // 15 KB
    float time_interval = 0.5;  // Arrives every 0.5 seconds
    float current_time = 0.0;
    
    // Calculate how many tokens are added per interval
    int tokens_per_interval = (int)(token_rate * time_interval); 

    printf("Time(s) | Tokens Added | Tokens Before | Packet(KB) | Status  | Tokens After\n");
    printf("------------------------------------------------------------------------------\n");

    while (1) {
        current_time += time_interval;

        // 1. Add tokens for this time interval
        int tokens_before = tokens + tokens_per_interval;
        
        // 2. Enforce maximum bucket size
        if (tokens_before > max_bucket_size) {
            tokens_before = max_bucket_size;
        }

        // 3. Process the packet
        if (tokens_before >= packet_size) {
            tokens = tokens_before - packet_size;
            printf("%-7.1f | %-12d | %-13d | %-10d | %-7s | %-12d\n", 
                   current_time, tokens_per_interval, tokens_before, packet_size, "Sent", tokens);
        } else {
            printf("%-7.1f | %-12d | %-13d | %-10d | %-7s | %-12d\n", 
                   current_time, tokens_per_interval, tokens_before, packet_size, "QUEUED", tokens_before);
            printf("\n--> Network congestion! Packet requires %d KB, but only %d KB available.\n", packet_size, tokens_before);
            break; // Stop simulation once queuing occurs
        }
    }

    return 0;
}