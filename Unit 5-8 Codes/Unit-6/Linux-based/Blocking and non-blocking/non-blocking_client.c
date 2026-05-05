#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    sock = socket(AF_INET, SOCK_STREAM, 0);

    // SET SOCKET TO NON-BLOCKING
    int flags = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, flags | O_NONBLOCK);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8085);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    // 1. Non-blocking Connect
    int res = connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    
    if (res < 0 && errno != EINPROGRESS) {
        perror("Connection Failed");
        return -1;
    }

    printf("Non-blocking connect initiated. Doing other work...\n");

    // 2. Polling for data
    while (1) {
        int bytes = recv(sock, buffer, sizeof(buffer), 0);
        
        if (bytes > 0) {
            printf("Received: %s\n", buffer);
            break;
        } else if (bytes == 0) {
            printf("Server closed connection.\n");
            break;
        } else {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                // No data yet
                printf("No data yet, client is still free to do other things...\n");
                sleep(5); 
            } else {
                // A real error occurred (like connection refused)
                // Note: in non-blocking, a failed connect often shows up here
                perror("recv error");
                break;
            }
        }
    }

    close(sock);
    return 0;
}