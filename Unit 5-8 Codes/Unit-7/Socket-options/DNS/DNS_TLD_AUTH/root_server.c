#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

int main()
{
    int sockfd;
    struct sockaddr_in server, client;
    char buffer[1024];
    socklen_t len = sizeof(client);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    server.sin_family = AF_INET;
    server.sin_port = htons(5050);
    server.sin_addr.s_addr = INADDR_ANY;

    bind(sockfd, (struct sockaddr *)&server, sizeof(server));
    printf("Root Server running on port 5050...\n");

    while (1)
    {
        recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client, &len);
        buffer[strcspn(buffer, "\n")] = '\0';
        printf("Query: %s\n", buffer);

        char *response = "Try a.gtld-server.com at port 5051";
        sendto(sockfd, response, strlen(response), 0, (struct sockaddr *)&client, len);
    }

    close(sockfd);
    return 0;
}
