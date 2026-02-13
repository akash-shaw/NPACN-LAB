#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main() {
    int sockfd, portno = 8080;
    struct sockaddr_in serv_addr;
    int choice, size, i;
    int arr[100];
    int search_key, found;
    int order; // 1 for Asc, 2 for Desc
    int odd_count, even_count;
    int odd_arr[100], even_arr[100];

    // 1. Create Socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    // 2. Initialize Server Address
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Localhost
    serv_addr.sin_port = htons(portno);

    // 3. Connect
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR connecting");
        exit(1);
    }
    printf("Connected to Server.\n");

    while (1) {
        printf("\n--- MENU ---\n");
        printf("1. Search for a number\n");
        printf("2. Sort (Ascending/Descending)\n");
        printf("3. Split into Odd and Even\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        // Send Choice
        write(sockfd, &choice, sizeof(int));

        if (choice == 4) {
            printf("Exiting...\n");
            break;
        }

        printf("Enter number of elements in the set: ");
        scanf("%d", &size);
        write(sockfd, &size, sizeof(int));

        printf("Enter %d integers:\n", size);
        for (i = 0; i < size; i++) {
            scanf("%d", &arr[i]);
        }
        write(sockfd, arr, size * sizeof(int));

        switch (choice) {
            case 1: // Search
                printf("Enter number to search: ");
                scanf("%d", &search_key);
                write(sockfd, &search_key, sizeof(int));

                read(sockfd, &found, sizeof(int));
                if (found != -1)
                    printf("Number found at position: %d\n", found);
                else
                    printf("Number not found in the set.\n");
                break;

            case 2: // Sort
                printf("Enter 1 for Ascending, 2 for Descending: ");
                scanf("%d", &order);
                write(sockfd, &order, sizeof(int));

                read(sockfd, arr, size * sizeof(int));
                printf("Sorted Set: ");
                for (i = 0; i < size; i++) printf("%d ", arr[i]);
                printf("\n");
                break;

            case 3: // Split
                read(sockfd, &odd_count, sizeof(int));
                read(sockfd, &even_count, sizeof(int));

                if (odd_count > 0) read(sockfd, odd_arr, odd_count * sizeof(int));
                if (even_count > 0) read(sockfd, even_arr, even_count * sizeof(int));

                printf("Odd Numbers: ");
                for (i = 0; i < odd_count; i++) printf("%d ", odd_arr[i]);
                printf("\nEven Numbers: ");
                for (i = 0; i < even_count; i++) printf("%d ", even_arr[i]);
                printf("\n");
                break;
            default:
                printf("Invalid choice.\n");
        }
    }

    close(sockfd);
    return 0;
}