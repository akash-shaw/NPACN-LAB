#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <signal.h>

#define PORT 8080

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr = { .sin_family = AF_INET, .sin_port = htons(PORT) };
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection Failed");
        return -1;
    }

    printf("Connected to server.\n");

    // Enable Full-Duplex: Fork client into Reader and Writer processes
    pid_t pid = fork();

    if (pid == 0) {
        // Child Process: Asynchronously Read from Server (Full-Duplex)
        char buffer[2048];
        while (1) {
            memset(buffer, 0, sizeof(buffer));
            int n = recv(sock, buffer, sizeof(buffer)-1, 0);
            if (n <= 0) {
                printf("\nServer disconnected.\n");
                kill(getppid(), SIGKILL); // Kill writer process if server drops
                exit(0);
            }
            
            // Check for broadcast termination (Task d)
            if (strstr(buffer, "terminate session") != NULL) {
                printf("\n[Server Broadcast]: terminate session\nExiting...\n");
                kill(getppid(), SIGKILL); // Kill writer process
                exit(0);
            }
            
            printf("\n[Server]: %s\n> ", buffer);
            fflush(stdout);
        }
    } else {
        // Parent Process: Read Input from User and Send
        char buffer[1024];
        while (1) {
            sleep(1); // Small delay to let server responses print cleanly before prompt
            printf("\n1. Add Employee\n2. Remove Duplicates by ID\n3. Display Sorted Employees\n> Enter choice: ");
            int choice;
            scanf("%d", &choice);
            getchar(); // Consume newline

            if (choice == 1) {
                char name[50]; int id, exp; float sal;
                printf("Enter Name, ID, Experience, Salary (space separated): ");
                scanf("%s %d %d %f", name, &id, &exp, &sal);
                sprintf(buffer, "A|%s|%d|%d|%.2f", name, id, exp, sal);
                send(sock, buffer, strlen(buffer), 0);
            } else if (choice == 2) {
                int id;
                printf("Enter Employee ID to remove duplicates: ");
                scanf("%d", &id);
                sprintf(buffer, "R|%d", id);
                send(sock, buffer, strlen(buffer), 0);
            } else if (choice == 3) {
                sprintf(buffer, "D|");
                send(sock, buffer, strlen(buffer), 0);
            } else {
                printf("Invalid choice.\n");
            }
        }
    }
    close(sock);
    return 0;
}