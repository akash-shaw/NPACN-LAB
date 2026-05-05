// client_socket_options.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define PORT 8080

int main() {
    int sock;
    struct sockaddr_in serv_addr;
    int recv_buf_size;
    socklen_t optlen = sizeof(recv_buf_size);
    char buffer[1024] = {0};

    // Step 1: Create a socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    printf("Client: Socket created successfully.\n");

    // Step 2: Set SO_RCVBUF (Receive Buffer Size)
    recv_buf_size = 32768; // 32 KB
    if (setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &recv_buf_size, sizeof(recv_buf_size)) < 0) {
        perror("setsockopt failed");
    } else {
        printf("Client: Updated receive buffer size (SO_RCVBUF) to: %d bytes\n", recv_buf_size);
    }

    // Step 3: Retrieve the current receive buffer size
    if (getsockopt(sock, SOL_SOCKET, SO_RCVBUF, &recv_buf_size, &optlen) < 0) {
        perror("getsockopt failed");
    } else {
        printf("Client: Current receive buffer size (SO_RCVBUF): %d bytes\n", recv_buf_size);
    }

    // Step 4: Connect to the server
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address or address not supported");
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        close(sock);
        exit(EXIT_FAILURE);
    }
    printf("Client: Connected to the server.\n");

    // Step 5: Receive message from the server
    int valread = read(sock, buffer, sizeof(buffer));
    if (valread > 0) {
        printf("Client: Message received from server: %s\n", buffer);
    }

    // Step 6: Close the socket
    close(sock);
    printf("Client: Connection closed.\n");

    return 0;
}
