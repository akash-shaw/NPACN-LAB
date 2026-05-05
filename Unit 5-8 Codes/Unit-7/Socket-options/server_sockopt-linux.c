
// server_socket_options_fixed.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>

#define PORT 8080

int main(void) {
    int server_fd = -1, new_socket = -1;
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    int on = 1;

    // 1) socket()
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) { perror("socket"); return EXIT_FAILURE; }
    printf("Server: socket() OK\n");

    // 2) SO_REUSEADDR: allow quick restart after TIME_WAIT
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
        perror("setsockopt(SO_REUSEADDR)"); goto cleanup;
    }
    printf("Server: SO_REUSEADDR set\n");

    // 2b) (Optional) SO_REUSEPORT: ONLY if you intend multiple binders to same IP:PORT
    // int on2 = 1;
    // if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &on2, sizeof(on2)) < 0) {
    //     perror("setsockopt(SO_REUSEPORT)"); goto cleanup;
    // }
    // printf("Server: SO_REUSEPORT set\n");

    // 3) bind()
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);     // explicit
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind");
        fprintf(stderr,
            "Tip: If this says EADDRINUSE, check for another listener on port %d\n", PORT);
        goto cleanup;
    }
    printf("Server: bound to 0.0.0.0:%d\n", PORT);

    // 4) listen()
    if (listen(server_fd, SOMAXCONN) < 0) { perror("listen"); goto cleanup; }
    printf("Server: listening...\n");

    // 5) accept()
    new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen);
    if (new_socket < 0) { perror("accept"); goto cleanup; }
    printf("Server: client connected\n");

    // 6) Show & tune SO_RCVBUF
    int rcvbuf = 0; socklen_t optlen = sizeof(rcvbuf);
    if (getsockopt(new_socket, SOL_SOCKET, SO_RCVBUF, &rcvbuf, &optlen) == 0)
        printf("Server: default SO_RCVBUF: %d bytes\n", rcvbuf);

    rcvbuf = 65536;
    if (setsockopt(new_socket, SOL_SOCKET, SO_RCVBUF, &rcvbuf, sizeof(rcvbuf)) == 0)
        printf("Server: updated SO_RCVBUF to %d bytes\n", rcvbuf);

    // 7) send
    const char *msg = "Hello from server!";
    if (send(new_socket, msg, strlen(msg), 0) >= 0)
        printf("Server: sent greeting\n");

cleanup:
    if (new_socket >= 0) close(new_socket);
    if (server_fd >= 0) close(server_fd);
    return 0;
}
