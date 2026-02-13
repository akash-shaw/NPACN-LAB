#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#define MAXSIZE 1024

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
    int sockfd, retval;
    struct sockaddr_in serveraddr;
    char buff[MAXSIZE];
    char data[100];
    char transmitted_data[100];
    char error_data[100];
    int parity_choice, ones, parity_bit;
    int recedbytes, sentbytes;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("\nSocket Creation Error");
        return -1;
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(8081);
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    retval = connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    if (retval == -1) {
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
    send(sockfd, &parity_choice, sizeof(int), 0);

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
    sentbytes = send(sockfd, error_data, strlen(error_data), 0);
    if (sentbytes == -1) {
        printf("Error sending data");
        close(sockfd);
    }
    printf("Data sent to server.\n");

    // Receive Result
    recedbytes = recv(sockfd, buff, MAXSIZE, 0);
    if (recedbytes == -1) {
         printf("Error receiving result");
         close(sockfd);
    }
    buff[recedbytes] = '\0';
    printf("\nServer Response: %s\n", buff);

    close(sockfd);
    return 0;
}
