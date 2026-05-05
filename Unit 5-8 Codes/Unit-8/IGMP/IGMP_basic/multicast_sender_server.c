#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MULTICAST_IP "239.1.1.1"
#define PORT 8085

int main() {
    int sockfd;
    struct sockaddr_in mcast_addr;
    char buffer[1024];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    mcast_addr.sin_family = AF_INET;
    mcast_addr.sin_addr.s_addr = inet_addr(MULTICAST_IP);
    mcast_addr.sin_port = htons(PORT);

    int ttl = 1;
    setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl));

    printf("Multicast Teleconference Server Started\n");

    while (1) {
        printf("Enter message (Ctrl+C to quit): ");
        fgets(buffer, sizeof(buffer), stdin);

        sendto(sockfd, buffer, strlen(buffer), 0,
               (struct sockaddr *)&mcast_addr, sizeof(mcast_addr));
    }

    close(sockfd);
    return 0;
}
