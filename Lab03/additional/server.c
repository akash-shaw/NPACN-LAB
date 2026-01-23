#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080
#define MAX_MAT 5 // Maximum matrix size for simulation

// --- Protocol Structure ---
// This defines the data format exchanged between Client and Server
typedef struct {
    int option;
    
    // Inputs
    int num1, num2;                 // For Option 1
    float a, b;                     // For Option 2 (ax + b = 0)
    int matA[MAX_MAT][MAX_MAT];     // For Option 3
    int matB[MAX_MAT][MAX_MAT];
    int r1, c1, r2, c2;

    // Outputs
    int sum, diff;                  // Result Option 1
    float x_val;                    // Result Option 2
    int matRes[MAX_MAT][MAX_MAT];   // Result Option 3
    int res_r, res_c;
    char error_msg[100];            // To handle errors (div by zero, dim mismatch)
} DataPacket;

void handle_client(int new_socket) {
    DataPacket packet;

    while (1) {
        // Read data from client
        int valread = read(new_socket, &packet, sizeof(packet));
        if (valread <= 0) break; // Client disconnected

        // Clear previous error messages
        strcpy(packet.error_msg, "");

        if (packet.option == 4) {
            printf("Client requested exit.\n");
            break;
        }

        switch (packet.option) {
            case 1: // Add and Subtract
                packet.sum = packet.num1 + packet.num2;
                packet.diff = packet.num1 - packet.num2;
                break;

            case 2: // Linear Equation: ax + b = 0 => x = -b/a
                if (packet.a == 0) {
                    strcpy(packet.error_msg, "Error: 'a' cannot be 0 in ax+b=0");
                } else {
                    packet.x_val = -packet.b / packet.a;
                }
                break;

            case 3: // Matrix Multiplication
                if (packet.c1 != packet.r2) {
                    strcpy(packet.error_msg, "Error: Matrix dimensions mismatch (cols A != rows B)");
                } else {
                    packet.res_r = packet.r1;
                    packet.res_c = packet.c2;
                    // Initialize result matrix to 0
                    for(int i=0; i<packet.res_r; i++)
                        for(int j=0; j<packet.res_c; j++)
                            packet.matRes[i][j] = 0;

                    // Multiplication logic
                    for (int i = 0; i < packet.r1; i++) {
                        for (int j = 0; j < packet.c2; j++) {
                            for (int k = 0; k < packet.c1; k++) {
                                packet.matRes[i][j] += packet.matA[i][k] * packet.matB[k][j];
                            }
                        }
                    }
                }
                break;
        }

        // Send the processed packet back to client
        send(new_socket, &packet, sizeof(packet), 0);
    }
    close(new_socket);
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int opt = 1;

    // 1. Create Socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // 2. Bind to Port
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // 3. Listen
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    // 4. Accept and Loop
    while(1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        printf("New client connected.\n");
        handle_client(new_socket);
    }
    return 0;
}