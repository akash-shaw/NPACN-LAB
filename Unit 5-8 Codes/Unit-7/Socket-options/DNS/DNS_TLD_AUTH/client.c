#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

int main()
{
    int sockfd;
    struct sockaddr_in server;
    char buffer[1024] = "What is the IP address of www.google.com?", response[1024];
    socklen_t len = sizeof(server);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    server.sin_family = AF_INET;
    server.sin_port = htons(5053);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&server, len);
    recvfrom(sockfd, response, sizeof(response), 0, (struct sockaddr *)&server, &len);
    printf("Final Response from Local DNS: %s\n", response);

    close(sockfd);
    return 0;
}
