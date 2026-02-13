#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8083

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    int data[30], code[30], m, r=0, i, j, k;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) return -1;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) return -1;

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
    // Start reading into a temp array or directly map
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
    write(sock, &total_len, sizeof(int));
    write(sock, code, sizeof(int) * 30); // Send whole array for simplicity

    // 7. Receive Result
    char buffer[1024] = {0};
    read(sock, buffer, 1024);
    printf("\nServer Response:\n%s\n", buffer);

    close(sock);
    return 0;
}
