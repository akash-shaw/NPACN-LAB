#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "unix_socket_chat"
#define BUFFER_SIZE 1024

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main() {
    int sock_fd;
    struct sockaddr_un server_addr;
    pid_t pid;
    char buffer[BUFFER_SIZE];

    // 1. Create Socket
    if ((sock_fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        error("Client: Socket creation failed");
    }

    // 2. Connect to Server
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        error("Client: Connection failed. Ensure server is running.");
    }

    printf("Client: Connected to server.\n");

    // 3. Fork to handle Full Duplex
    pid = fork();

    if (pid < 0) {
        error("Client: Fork failed");
    }

    if (pid == 0) {
        // CHILD PROCESS (Receiver)
        // Reads from Socket, Writes to Screen
        printf("[Child] PID: %d, PPID: %d - Ready to RECEIVE\n", getpid(), getppid());
        
        while (1) {
            memset(buffer, 0, BUFFER_SIZE);
            int bytes_read = read(sock_fd, buffer, BUFFER_SIZE);
            if (bytes_read <= 0) {
                printf("Server disconnected or error.\n");
                break;
            }
            printf("\nServer: %sClient (You): ", buffer);
            fflush(stdout);
        }
        close(sock_fd);
        exit(0);
    } else {
        // PARENT PROCESS (Sender)
        // Reads from Keyboard, Writes to Socket
        printf("[Parent] PID: %d, PPID: %d - Ready to SEND\n", getpid(), getppid());
        
        while (1) {
            printf("Client (You): ");
            fgets(buffer, BUFFER_SIZE, stdin);
            write(sock_fd, buffer, strlen(buffer));

            if (strncmp(buffer, "exit", 4) == 0) {
                break;
            }
        }
        close(sock_fd);
        exit(0);
    }

    return 0;
}