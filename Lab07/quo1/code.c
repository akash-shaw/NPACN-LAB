#include <stdio.h>

int main() {
    // Problem parameters
    int arrival_times[] = {1, 2, 3, 5, 6, 8, 11, 12, 15, 16, 19};
    int num_packets = sizeof(arrival_times) / sizeof(arrival_times[0]);
    int packet_size = 4;
    int bucket_size = 10;
    int outgoing_rate = 1;

    // State variables
    int bucket_content = 0;
    int last_time = 0;

    printf("--- Leaky Bucket Congestion Control Simulation ---\n");
    printf("Bucket Size: %d bytes | Packet Size: %d bytes | Outgoing Rate: %d byte/s\n\n", 
           bucket_size, packet_size, outgoing_rate);

    // Process each incoming packet
    for (int i = 0; i < num_packets; i++) {
        int current_time = arrival_times[i];
        
        // Calculate the time elapsed since the last packet arrival
        int time_elapsed = current_time - last_time;
        
        // Leak bytes from the bucket based on the time elapsed
        bucket_content -= time_elapsed * outgoing_rate;
        
        // The bucket content cannot be negative
        if (bucket_content < 0) {
            bucket_content = 0;
        }

        printf("Time = %2d s: ", current_time);

        // Check if the arriving packet fits into the bucket
        if (bucket_content + packet_size <= bucket_size) {
            // Packet is conforming (accepted)
            bucket_content += packet_size;
            printf("Packet CONFORMING (Accepted). Bucket contains %2d/%d bytes.\n", 
                   bucket_content, bucket_size);
        } else {
            // Packet is nonconforming (dropped)
            printf("Packet NONCONFORMING (Dropped). Bucket remains  %2d/%d bytes.\n", 
                   bucket_content, bucket_size);
        }

        // Update the last_time for the next iteration
        last_time = current_time;
    }

    return 0;
}