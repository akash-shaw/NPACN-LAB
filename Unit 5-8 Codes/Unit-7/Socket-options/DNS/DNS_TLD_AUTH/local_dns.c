#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

void query_server(char *server_ip, int port, char *msg, char *response)
{
    int sockfd;
    struct sockaddr_in server;
    socklen_t len = sizeof(server);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(server_ip);

    sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr *)&server, len);
    recvfrom(sockfd, response, 1024, 0, (struct sockaddr *)&server, &len);
    close(sockfd);
}

int main()
{
    int sockfd;
    struct sockaddr_in local, client;
    char buffer[1024], response[1024];
    socklen_t len = sizeof(client);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    local.sin_family = AF_INET;
    local.sin_port = htons(5053);
    local.sin_addr.s_addr = INADDR_ANY;

    bind(sockfd, (struct sockaddr *)&local, sizeof(local));
    printf("Local DNS running on port 5053...\n");

    while (1)
    {
        recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client, &len);
        buffer[strcspn(buffer, "\n")] = '\0';
        printf("Client query: %s\n", buffer);

        // Step 1: Query root
        query_server("127.0.0.1", 5050, buffer, response);
        printf("Root Response: %s\n", response);

        // Step 2: Query TLD
        query_server("127.0.0.1", 5051, buffer, response);
        printf("TLD Response: %s\n", response);

        // Step 3: Query Authoritative
        query_server("127.0.0.1", 5052, buffer, response);
        printf("Authoritative Response: %s\n", response);

        // Send final response to client
        sendto(sockfd, response, strlen(response), 0, (struct sockaddr *)&client, len);
    }

    close(sockfd);
    return 0;
}
