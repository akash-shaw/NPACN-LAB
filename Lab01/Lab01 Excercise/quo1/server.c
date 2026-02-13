#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

void sort_array(int *arr, int n, int ascending) {
    int i, j, temp;
    for (i = 0; i < n - 1; i++) {
        for (j = 0; j < n - i - 1; j++) {
            if (ascending) {
                if (arr[j] > arr[j + 1]) {
                    temp = arr[j];
                    arr[j] = arr[j + 1];
                    arr[j + 1] = temp;
                }
            } else {
                if (arr[j] < arr[j + 1]) {
                    temp = arr[j];
                    arr[j] = arr[j + 1];
                    arr[j + 1] = temp;
                }
            }
        }
    }
}

int main() {
    int sockfd, newsockfd, portno = 8080;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    int n, choice, size, i;
    int buffer[100]; // Buffer to store the set of integers
    int result_buffer[100];
    int search_key, found, odd_count, even_count;
    int odd_arr[100], even_arr[100];

    // 1. Create Socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    // 2. Initialize Socket Structure
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    // 3. Bind
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
        exit(1);
    }

    // 4. Listen
    listen(sockfd, 5);
    printf("Server listening on port %d...\n", portno);

    clilen = sizeof(cli_addr);

    // 5. Accept
    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    if (newsockfd < 0) {
        perror("ERROR on accept");
        exit(1);
    }
    printf("Client connected.\n");

    // Loop to receive messages until "exit"
    while (1) {
        // Read Choice
        n = read(newsockfd, &choice, sizeof(int));
        if (n <= 0) break; // Client disconnected

        if (choice == 4) { // Exit choice
            printf("Client requested exit. Terminating session.\n");
            break;
        }

        // Read Array Size
        read(newsockfd, &size, sizeof(int));

        // Read Array Elements
        read(newsockfd, buffer, size * sizeof(int));

        printf("Received request: Choice %d, Size %d\n", choice, size);

        switch (choice) {
            case 1: // Search
                read(newsockfd, &search_key, sizeof(int)); // Read search key
                found = -1; // -1 indicates not found
                for (i = 0; i < size; i++) {
                    if (buffer[i] == search_key) {
                        found = i + 1; // Return position (1-based)
                        break;
                    }
                }
                write(newsockfd, &found, sizeof(int));
                break;

            case 2: // Sort
                {
                    int order;
                    read(newsockfd, &order, sizeof(int)); // 1 for Asc, 2 for Desc
                    sort_array(buffer, size, (order == 1));
                    write(newsockfd, buffer, size * sizeof(int));
                }
                break;

            case 3: // Split Odd/Even
                odd_count = 0;
                even_count = 0;
                for (i = 0; i < size; i++) {
                    if (buffer[i] % 2 != 0)
                        odd_arr[odd_count++] = buffer[i];
                    else
                        even_arr[even_count++] = buffer[i];
                }
                // Send Counts first
                write(newsockfd, &odd_count, sizeof(int));
                write(newsockfd, &even_count, sizeof(int));
                // Send Arrays
                if (odd_count > 0) write(newsockfd, odd_arr, odd_count * sizeof(int));
                if (even_count > 0) write(newsockfd, even_arr, even_count * sizeof(int));
                break;
        }
    }

    close(newsockfd);
    close(sockfd);
    return 0;
}