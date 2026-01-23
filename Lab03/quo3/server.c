#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Comparator for Ascending Order (integers/digits)
int compareAsc(const void *a, const void *b) {
    return (*(char *)a - *(char *)b);
}

// Comparator for Descending Order (characters)
int compareDesc(const void *a, const void *b) {
    return (*(char *)b - *(char *)a);
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    // 1. Create socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // 2. Attach socket to the port 8080
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

    // 3. Listen for connections
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d...\n", PORT);

    // 4. Accept a connection
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    // 5. Read the alphanumeric string from client
    read(new_socket, buffer, BUFFER_SIZE);
    
    // 6. Fork the process
    pid_t pid = fork();

    if (pid == 0) {
        // --- CHILD PROCESS ---
        // Task: Sort numbers in ascending order
        char digits[BUFFER_SIZE] = {0};
        int d_idx = 0;

        // Extract digits
        for (int i = 0; i < strlen(buffer); i++) {
            if (isdigit(buffer[i])) {
                digits[d_idx++] = buffer[i];
            }
        }

        // Sort digits ascending
        qsort(digits, d_idx, sizeof(char), compareAsc);

        // Prepare and send message
        char msg[BUFFER_SIZE];
        sprintf(msg, "Output at the child process of the server (PID %d): %s\n", getpid(), digits);
        send(new_socket, msg, strlen(msg), 0);
        
        // Child closes its copy of the socket and exits
        close(new_socket);
        exit(0);

    } else if (pid > 0) {
        // --- PARENT PROCESS ---
        
        // Wait for child to finish first (to ensure output order matches sample)
        wait(NULL);

        // Task: Sort characters in descending order
        char chars[BUFFER_SIZE] = {0};
        int c_idx = 0;

        // Extract alphabets
        for (int i = 0; i < strlen(buffer); i++) {
            if (isalpha(buffer[i])) {
                chars[c_idx++] = buffer[i];
            }
        }

        // Sort characters descending
        qsort(chars, c_idx, sizeof(char), compareDesc);

        // Prepare and send message
        char msg[BUFFER_SIZE];
        sprintf(msg, "Output at the parent process of the server (PID %d): %s\n", getpid(), chars);
        send(new_socket, msg, strlen(msg), 0);

        // Parent closes socket
        close(new_socket);
    } else {
        perror("fork failed");
    }

    close(server_fd);
    return 0;
}