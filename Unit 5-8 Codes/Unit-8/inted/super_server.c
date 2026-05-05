#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>

#define MAX_PORTS 3

int ports[MAX_PORTS] = {9001, 9002, 9003};
char *services[MAX_PORTS] = {
    "./echo_server",
    "./time_server",
    "./upper_server"
};

void reap_child(int sig) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

int main() {
    int sockfd[MAX_PORTS];
    struct sockaddr_in addr;
    fd_set readfds;
    int maxfd = 0;

    signal(SIGCHLD, reap_child);

    for (int i = 0; i < MAX_PORTS; i++) {
        sockfd[i] = socket(AF_INET, SOCK_STREAM, 0);

        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(ports[i]);

        bind(sockfd[i], (struct sockaddr *)&addr, sizeof(addr));
        listen(sockfd[i], 5);

        if (sockfd[i] > maxfd)
            maxfd = sockfd[i];

        printf("Super Server listening on port %d\n", ports[i]);
    }

    while (1) {
        FD_ZERO(&readfds);

        for (int i = 0; i < MAX_PORTS; i++)
            FD_SET(sockfd[i], &readfds);

        select(maxfd + 1, &readfds, NULL, NULL, NULL);

        for (int i = 0; i < MAX_PORTS; i++) {
            if (FD_ISSET(sockfd[i], &readfds)) {
                int client = accept(sockfd[i], NULL, NULL);

                if (fork() == 0) {
                    dup2(client, STDIN_FILENO);
                    dup2(client, STDOUT_FILENO);

                    close(client);
                    execv(services[i], NULL);

                    perror("exec failed");
                    exit(1);
                }
                close(client);
            }
        }
    }
}
