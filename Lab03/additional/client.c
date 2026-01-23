#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_MAT 5

// --- Protocol Structure (Must match Server) ---
typedef struct {
    int option;
    
    // Inputs
    int num1, num2;
    float a, b;
    int matA[MAX_MAT][MAX_MAT];
    int matB[MAX_MAT][MAX_MAT];
    int r1, c1, r2, c2;

    // Outputs
    int sum, diff;
    float x_val;
    int matRes[MAX_MAT][MAX_MAT];
    int res_r, res_c;
    char error_msg[100];
} DataPacket;

void read_matrix(int mat[MAX_MAT][MAX_MAT], int r, int c) {
    printf("Enter elements:\n");
    for (int i = 0; i < r; i++) {
        for (int j = 0; j < c; j++) {
            scanf("%d", &mat[i][j]);
        }
    }
}

void print_matrix(int mat[MAX_MAT][MAX_MAT], int r, int c) {
    for (int i = 0; i < r; i++) {
        for (int j = 0; j < c; j++) {
            printf("%d\t", mat[i][j]);
        }
        printf("\n");
    }
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    DataPacket packet;

    // 1. Create Socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    // 2. Connect
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    int choice;
    while (1) {
        printf("\n--- MENU ---\n");
        printf("1. Add/Subtract two integers\n");
        printf("2. Find 'x' in linear equation (ax + b = 0)\n");
        printf("3. Multiply two matrices\n");
        printf("4. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        packet.option = choice;

        if (choice == 4) {
            send(sock, &packet, sizeof(packet), 0);
            break;
        }

        // Input Gathering
        if (choice == 1) {
            printf("Enter two integers: ");
            scanf("%d %d", &packet.num1, &packet.num2);
        } 
        else if (choice == 2) {
            printf("For equation ax + b = 0, enter a and b: ");
            scanf("%f %f", &packet.a, &packet.b);
        } 
        else if (choice == 3) {
            printf("Enter rows and cols for Matrix A: ");
            scanf("%d %d", &packet.r1, &packet.c1);
            read_matrix(packet.matA, packet.r1, packet.c1);

            printf("Enter rows and cols for Matrix B: ");
            scanf("%d %d", &packet.r2, &packet.c2);
            read_matrix(packet.matB, packet.r2, packet.c2);
        }

        // 3. Send Data
        send(sock, &packet, sizeof(packet), 0);

        // 4. Receive Result
        read(sock, &packet, sizeof(packet));

        // Display Output
        if (strlen(packet.error_msg) > 0) {
            printf("SERVER ERROR: %s\n", packet.error_msg);
        } else {
            if (choice == 1) {
                printf("Server Result:\n Sum: %d\n Difference: %d\n", packet.sum, packet.diff);
            } else if (choice == 2) {
                printf("Server Result:\n x = %.2f\n", packet.x_val);
            } else if (choice == 3) {
                printf("Server Result (Matrix Multiplication):\n");
                print_matrix(packet.matRes, packet.res_r, packet.res_c);
            }
        }
    }

    close(sock);
    return 0;
}