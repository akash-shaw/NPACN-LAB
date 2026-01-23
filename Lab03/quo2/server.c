#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Function to swap characters
void swap(char *x, char *y) {
    char temp;
    temp = *x;
    *x = *y;
    *y = temp;
}

// Function to print permutations
void permute(char *a, int l, int r) {
    int i;
    if (l == r) {
        printf("%s ", a);
    } else {
        for (i = l; i <= r; i++) {
            swap((a + l), (a + i));
            permute(a, l + 1, r);
            swap((a + l), (a + i)); // Backtrack
        }
    }
}

int main() {
    int sockfd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    // 1. Create socket file descriptor (AF_INET = IPv4, SOCK_DGRAM = UDP/Connectionless)
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));

    // 2. Fill server information
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // 3. Bind the socket with the server address
    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    printf("Server is running on port %d (UDP)...\n", PORT);

    while (1) {
        printf("\nWaiting for data...\n");

        // 4. Receive message from client
        int n = recvfrom(sockfd, (char *)buffer, BUFFER_SIZE, MSG_WAITALL, 
                        (struct sockaddr *)&client_addr, &addr_len);
        buffer[n] = '\0'; // Null-terminate the received string

        printf("Client sent: %s\n", buffer);
        printf("Permutations: ");
        
        // 5. Calculate and print permutations
        permute(buffer, 0, strlen(buffer) - 1);
        printf("\n");

        // 6. Send acknowledgement back to client (Half-Duplex response)
        const char *ack = "Server processed your string.";
        sendto(sockfd, ack, strlen(ack), MSG_CONFIRM, 
               (const struct sockaddr *)&client_addr, addr_len);
    }

    close(sockfd);
    return 0;
}