#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#define PORT 9001
#define MAX 1024

void handle_request(int new_socket) {
    char domain[MAX] = {0};
    char db_domain[MAX], db_ip[MAX];
    char response[MAX] = "Domain Not Found";

    int n = read(new_socket, domain, MAX - 1);
    if (n > 0) {
        domain[n] = '\0';
        domain[strcspn(domain, "\r\n")] = 0;

        printf("Searching for: [%s]\n", domain);

        FILE *fp = fopen("database.txt", "r");
        if (fp != NULL) {
            while (fscanf(fp, "%s %s", db_domain, db_ip) != EOF) {
                if (strcmp(domain, db_domain) == 0) {
                    strcpy(response, db_ip);
                    break;
                }
            }
            fclose(fp);
        } else {
            strcpy(response, "Server Error: database.txt missing");
        }
    }

    send(new_socket, response, strlen(response), 0);
    close(new_socket);
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 5);

    printf("DNS Server running on port %d...\n", PORT);

    while (1) {
        new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);

        if (fork() == 0) {
            close(server_fd);
            handle_request(new_socket);
            exit(0);
        }

        close(new_socket);
        waitpid(-1, NULL, WNOHANG);
    }
}
