#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 9090
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in serv_addr;
    int option;
    char input_data[BUFFER_SIZE];
    char response[BUFFER_SIZE];

    while (1) {
        // 1. Create Socket
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            perror("ERROR opening socket");
            exit(1);
        }

        // 2. Connect
        bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        serv_addr.sin_port = htons(PORT);

        if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
            perror("ERROR connecting");
            exit(1);
        }

        // 3. User Menu
        printf("\n--- Student Info System ---\n");
        printf("1. Registration Number\n");
        printf("2. Name of Student\n");
        printf("3. Subject Code\n");
        printf("4. Exit\n");
        printf("Enter Option: ");
        scanf("%d", &option);

        if (option == 4) {
            close(sockfd);
            break;
        }

        printf("Enter Required Detail: ");
        scanf(" %[^\n]", input_data); // Read string with spaces

        // 4. Send Option and Data
        write(sockfd, &option, sizeof(int));
        write(sockfd, input_data, strlen(input_data) + 1);

        // 5. Receive Response
        memset(response, 0, BUFFER_SIZE);
        if (read(sockfd, response, BUFFER_SIZE) > 0) {
            printf("\n--- Server Response ---\n%s\n", response);
        }

        close(sockfd); // Close connection after transaction
    }
    return 0;
}