#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 9001
#define MAX 1024

int main() {
    char domain[MAX], buffer[MAX];
    struct sockaddr_in serv_addr;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    while (1) {
        printf("\nEnter Domain (or type 'exit'): ");
        scanf("%s", domain);

        if (strcmp(domain, "exit") == 0)
            break;

        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0)
            continue;

        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
            close(sock);
            continue;
        }

        send(sock, domain, strlen(domain), 0);

        memset(buffer, 0, MAX);
        read(sock, buffer, MAX - 1);
        buffer[strcspn(buffer, "\r\n")] = 0;

        printf("Server Response (IP): %s\n", buffer);

        if (strlen(buffer) > 0 && strcmp(buffer, "Domain Not Found") != 0) {

            char url[1200];
            snprintf(url, sizeof(url), "http://%s", buffer);

            if (fork() == 0) {
                freopen("/dev/null", "w", stdout);                       // if browser is not opened means xdg package is not installed in os. Especially in WSL
                freopen("/dev/null", "w", stderr);
                execlp("xdg-open", "xdg-open", url, (char *)NULL);    //sudo apt update && sudo apt install xdg-utils
                exit(0);
            }
        }

        close(sock);
    }

    return 0;
}
