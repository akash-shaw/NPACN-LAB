#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define MAXSIZE 1024
#define PORT 8083

int main() {
    int sockfd, retval;
    struct sockaddr_in serveraddr;
    int data[30], code[30];
    int m, r=0, i, j, k;
    int recedbytes, sentbytes;
    char buff[MAXSIZE];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("\nSocket Creation Error");
        return 0;
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(PORT);
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    retval = connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    if (retval == -1) {
        printf("\nConnection Failed \n");
        close(sockfd);
        return 0;
    }

    // --- SENDER SIDE ---
    printf("\n--- HAMMING CODE CLIENT ---\n");
    printf("Enter number of data bits: ");
    scanf("%d", &m);

    // 1. Calculate r
    while (pow(2, r) < (m + r + 1)) {
        r++;
    }
    printf("Number of redundancy bits (r): %d\n", r);
    
    // 2. Input Data
    printf("Enter data bits (D1..Dm):\n");
    // Start reading into a temp array
    int temp_data[30];
    for(i=1; i<=m; i++) scanf("%d", &temp_data[i]);
    
    // 3. Construct Code
    int total_len = m + r;
    j = 1; 
    k = 0;
    for (i = 1; i <= total_len; i++) {
        if (i == pow(2, k)) {
            code[i] = 0; k++;
        } else {
            code[i] = temp_data[j++];
        }
    }

    // 4. Calculate Parity
    for (i = 0; i < r; i++) {
        int position = pow(2, i);
        int count = 0;
        for (j = position; j <= total_len; j++) {
            if (((j >> i) & 1) == 1) {
                if (code[j] == 1) count++;
            }
        }
        if (count % 2 != 0) code[position] = 1;
        else code[position] = 0;
    }

    printf("Generated Code Word: ");
    for(i=1; i<=total_len; i++) printf("%d", code[i]);
    printf("\n");

    // 5. Error Injection
    printf("Inject Error? Enter Index (0 for none): ");
    int err_idx;
    scanf("%d", &err_idx);
    if(err_idx > 0 && err_idx <= total_len) {
        code[err_idx] = (code[err_idx] == 0) ? 1 : 0;
        printf("Error Injected at %d. New Code: ", err_idx);
        for(i=1; i<=total_len; i++) printf("%d", code[i]);
        printf("\n");
    }

    // 6. Send
    sentbytes = send(sockfd, &total_len, sizeof(int), 0);
    if(sentbytes == -1) {
        printf("Error sending len");
        close(sockfd);
        return 0;
    }
    
    sentbytes = send(sockfd, code, sizeof(int) * 30, 0); // Send whole array for simplicity
    if(sentbytes == -1) {
        printf("Error sending code");
        close(sockfd);
        return 0;
    }

    // 7. Receive Result
    recedbytes = recv(sockfd, buff, MAXSIZE, 0);
    if(recedbytes == -1) {
        printf("Error receiving");
        close(sockfd);
        return 0;
    }
    buff[recedbytes] = '\0';
    printf("\nServer Response:\n%s\n", buff);

    close(sockfd);
    return 0;
}
