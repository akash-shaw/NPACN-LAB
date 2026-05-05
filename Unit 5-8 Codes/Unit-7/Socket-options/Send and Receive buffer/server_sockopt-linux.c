// server_socket_options.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define PORT 8080

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1, recv_buf_size;
    socklen_t optlen = sizeof(recv_buf_size);
    int addrlen = sizeof(address);

    // Step 1: Create a socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    printf("Server: Socket created successfully.\n");

    // Step 2: Set SO_REUSEADDR option
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Server: SO_REUSEADDR option set successfully.\n");

    // Step 3: Bind the socket to a port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Server: Socket bound to port %d.\n", PORT);

    // Step 4: Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Server: Listening for connections...\n");

    // Step 5: Accept a client connection
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
        perror("Accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Server: Client connected.\n");

    // Step 6: Retrieve the receive buffer size (SO_RCVBUF)
    if (getsockopt(new_socket, SOL_SOCKET, SO_RCVBUF, &recv_buf_size, &optlen) < 0) {
        perror("getsockopt failed");
    } else {
        printf("Server: Default receive buffer size (SO_RCVBUF): %d bytes\n", recv_buf_size);
    }

    // Step 7: Modify the receive buffer size
    recv_buf_size = 65536; // 64 KB
    if (setsockopt(new_socket, SOL_SOCKET, SO_RCVBUF, &recv_buf_size, sizeof(recv_buf_size)) < 0) {
        perror("setsockopt failed");
    } else {
        printf("Server: Updated receive buffer size (SO_RCVBUF) to: %d bytes\n", recv_buf_size);
    }

    // Step 8: Send a message to the client
    const char *message = "Hello from server!";
    send(new_socket, message, strlen(message), 0);
    printf("Server: Message sent to client.\n");

    // Step 9: Close sockets
    close(new_socket);
    close(server_fd);
    printf("Server: Connection closed.\n");

    return 0;
}
