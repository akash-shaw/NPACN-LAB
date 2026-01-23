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
    int server_fd, client_fd;
    struct sockaddr_un server_addr, client_addr;
    socklen_t client_len;
    pid_t pid;
    char buffer[BUFFER_SIZE];

    // 1. Create Socket
    // AF_UNIX for local communication, SOCK_STREAM for connection-oriented
    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        error("Server: Socket creation failed");
    }

    // 2. Bind Socket
    // Unlink ensures we don't get an "Address already in use" error
    unlink(SOCKET_PATH); 
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        error("Server: Bind failed");
    }

    // 3. Listen
    if (listen(server_fd, 5) < 0) {
        error("Server: Listen failed");
    }

    printf("Server: Waiting for client connection...\n");

    // 4. Accept Connection
    client_len = sizeof(client_addr);
    client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
    if (client_fd < 0) {
        error("Server: Accept failed");
    }

    printf("Server: Client connected.\n");

    // 5. Fork to handle Full Duplex
    pid = fork();

    if (pid < 0) {
        error("Server: Fork failed");
    }

    if (pid == 0) {
        // CHILD PROCESS (Receiver)
        // Reads from Socket, Writes to Screen
        printf("[Child] PID: %d, PPID: %d - Ready to RECEIVE\n", getpid(), getppid());
        
        while (1) {
            memset(buffer, 0, BUFFER_SIZE);
            int bytes_read = read(client_fd, buffer, BUFFER_SIZE);
            if (bytes_read <= 0) {
                printf("Client disconnected or error.\n");
                break;
            }
            printf("\nClient: %sServer (You): ", buffer);
            fflush(stdout); // Force print to keep prompt clean
        }
        close(client_fd);
        exit(0);
    } else {
        // PARENT PROCESS (Sender)
        // Reads from Keyboard, Writes to Socket
        printf("[Parent] PID: %d, PPID: %d - Ready to SEND\n", getpid(), getppid());
        
        while (1) {
            printf("Server (You): ");
            fgets(buffer, BUFFER_SIZE, stdin);
            write(client_fd, buffer, strlen(buffer));
            
            if (strncmp(buffer, "exit", 4) == 0) {
                break;
            }
        }
        close(client_fd);
        close(server_fd);
        unlink(SOCKET_PATH); // Clean up the socket file
        exit(0);
    }

    return 0;
}