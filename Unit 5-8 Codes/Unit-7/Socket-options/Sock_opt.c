#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main() {
    int sockfd;
    int opt = 1;
    int recv_buf_size;
    socklen_t optlen;

    // Step 1: Create a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    printf("Socket created successfully.\n");

    // Step 2: Use setsockopt to set SO_REUSEADDR
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    printf("Socket option SO_REUSEADDR set successfully.\n");

    // Step 3: Use getsockopt to retrieve SO_RCVBUF (Receive Buffer Size)
    optlen = sizeof(recv_buf_size);
    if (getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &recv_buf_size, &optlen) < 0) {
        perror("getsockopt failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    printf("Default receive buffer size (SO_RCVBUF): %d bytes\n", recv_buf_size);

    // Step 4: Modify the receive buffer size using setsockopt
    recv_buf_size = 65536; // Set buffer size to 64 KB
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &recv_buf_size, sizeof(recv_buf_size)) < 0) {
        perror("Failed to set receive buffer size");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    printf("Receive buffer size updated to: %d bytes\n", recv_buf_size);

    // Step 5: Verify the new buffer size using getsockopt
    if (getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &recv_buf_size, &optlen) < 0) {
        perror("getsockopt failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    printf("Updated receive buffer size (SO_RCVBUF): %d bytes\n", recv_buf_size);

    // Step 6: Close the socket
    close(sockfd);
    printf("Socket closed.\n");

    return 0;
}
