#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    // SET SOCKET TO NON-BLOCKING
    int flags = fcntl(server_fd, F_GETFL, 0);
    fcntl(server_fd, F_SETFL, flags | O_NONBLOCK);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8085);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 3);

    printf("Non-Blocking Server: Polling for connections...\n");

    while (1) {
        new_socket = accept(server_fd, NULL, NULL);
        if (new_socket < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // No connection yet, do other work here
                printf("No client yet... doing other tasks...\n");
                sleep(5); 
                continue;
            } else {
                perror("accept failed");
                break;
            }
        }
        
        printf("Client connected successfully!\n");
        close(new_socket);
        break;
    }

    close(server_fd);
    return 0;
}