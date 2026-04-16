#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>

#define PORT 8080
#define MAX_EMP 100

// Employee Database Structure
typedef struct {
    char name[50];
    int emp_id;
    int exp;
    float salary;
} Employee;

// Shared Memory Structure
typedef struct {
    Employee db[MAX_EMP];
    int count;
    sem_t lock; // POSIX Semaphore for inter-process synchronization
} SharedDB;

// Function to handle individual client requests (Task a, b, c)
void handle_client(int client_sock, struct sockaddr_in client_addr, SharedDB *db) {
    char buffer[1024];
    
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int n = recv(client_sock, buffer, sizeof(buffer)-1, 0);
        if (n <= 0) break; // Client disconnected
        
        char response[2048] = {0};

        if (buffer[0] == 'A') {
            // Task (a): Add Employee
            Employee new_emp;
            sscanf(buffer, "A|%[^|]|%d|%d|%f", new_emp.name, &new_emp.emp_id, &new_emp.exp, &new_emp.salary);
            
            sem_wait(&db->lock); // Lock Shared Memory
            db->db[db->count++] = new_emp;
            sem_post(&db->lock); // Unlock Shared Memory

            // Send back client info and child PID so it displays on the client side
            sprintf(response, "Added successfully! Client IP: %s, Port: %d, Server Child PID: %d\n", 
                    inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), getpid());
            send(client_sock, response, strlen(response), 0);

        } else if (buffer[0] == 'R') {
            // Task (b): Remove Duplicates by ID
            int target_id;
            sscanf(buffer, "R|%d", &target_id);
            
            FILE *f = fopen("dup.txt", "a");
            int first_seen = 0;
            int removed = 0;

            sem_wait(&db->lock); // Lock Shared Memory
            for (int i = 0; i < db->count; ) {
                if (db->db[i].emp_id == target_id) {
                    if (!first_seen) {
                        first_seen = 1; // Keep the first occurrence
                        i++;
                    } else {
                        // Duplicate found: write to file and remove from array
                        fprintf(f, "Name: %s, ID: %d, Exp: %d, Salary: %.2f\n", 
                                db->db[i].name, db->db[i].emp_id, db->db[i].exp, db->db[i].salary);
                        removed++;
                        for (int j = i; j < db->count - 1; j++) {
                            db->db[j] = db->db[j+1]; // Shift left
                        }
                        db->count--;
                    }
                } else {
                    i++;
                }
            }
            sem_post(&db->lock); // Unlock Shared Memory
            fclose(f);

            sprintf(response, "Duplicate removal complete. Removed %d duplicates.\n", removed);
            send(client_sock, response, strlen(response), 0);

        } else if (buffer[0] == 'D') {
            // Task (c): Sort and Display
            sem_wait(&db->lock); // Lock Shared Memory
            int n_emps = db->count;
            Employee temp[MAX_EMP];
            for (int i = 0; i < n_emps; i++) temp[i] = db->db[i];
            sem_post(&db->lock); // Unlock Shared Memory

            // Bubble Sort: Descending experience, Ascending ASCII name
            for (int i = 0; i < n_emps - 1; i++) {
                for (int j = 0; j < n_emps - i - 1; j++) {
                    if (temp[j].exp < temp[j+1].exp || 
                       (temp[j].exp == temp[j+1].exp && strcmp(temp[j].name, temp[j+1].name) > 0)) {
                        Employee t = temp[j];
                        temp[j] = temp[j+1];
                        temp[j+1] = t;
                    }
                }
            }

            strcpy(response, "\n--- Sorted Employees ---\n");
            for (int i = 0; i < n_emps; i++) {
                char line[128];
                sprintf(line, "Name: %s, ID: %d, Exp: %d, Salary: %.2f\n", 
                        temp[i].name, temp[i].emp_id, temp[i].exp, temp[i].salary);
                strcat(response, line);
            }
            send(client_sock, response, strlen(response), 0);
        }
    }
    close(client_sock);
    exit(0);
}

int main() {
    // Setup Shared Memory for Database
    SharedDB *db = mmap(NULL, sizeof(SharedDB), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    
    // Initialize POSIX unnamed semaphore for processes (pshared = 1, initial_value = 1)
    if (sem_init(&db->lock, 1, 1) == -1) {
        perror("Semaphore initialization failed");
        exit(1);
    }

    // Initial database population (at least 3 records)
    db->count = 3;
    strcpy(db->db[0].name, "Alice"); db->db[0].emp_id = 101; db->db[0].exp = 5; db->db[0].salary = 75000;
    strcpy(db->db[1].name, "Bob"); db->db[1].emp_id = 102; db->db[1].exp = 3; db->db[1].salary = 60000;
    strcpy(db->db[2].name, "Charlie"); db->db[2].emp_id = 101; db->db[2].exp = 5; db->db[2].salary = 72000; // Dup ID test

    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr = { .sin_family = AF_INET, .sin_port = htons(PORT), .sin_addr.s_addr = INADDR_ANY };
    
    int opt = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_sock, 5);
    printf("Server listening on port %d...\n", PORT);

    int client_sockets[10];
    pid_t child_pids[10];
    int active_clients = 0;

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        int new_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_len);

        client_sockets[active_clients] = new_sock;

        // Task (d): If more than 2 clients are connected, broadcast and terminate
        if (active_clients == 2) { // 0, 1, 2 -> meaning this is the 3rd connection
            printf("More than 2 clients connected. Broadcasting termination and shutting down.\n");
            for (int i = 0; i <= active_clients; i++) {
                send(client_sockets[i], "terminate session", 17, 0);
                close(client_sockets[i]);
            }
            // Kill existing child processes handling the previous clients
            for (int i = 0; i < active_clients; i++) {
                kill(child_pids[i], SIGKILL);
            }
            break; // Exit server loop
        }

        pid_t pid = fork();
        if (pid == 0) {
            close(server_sock);
            handle_client(new_sock, client_addr, db);
        } else {
            child_pids[active_clients] = pid;
            active_clients++;
        }
    }
    
    // Cleanup shared memory and semaphores
    sem_destroy(&db->lock);
    munmap(db, sizeof(SharedDB));
    close(server_sock);
    return 0;
}