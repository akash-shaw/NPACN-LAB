#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MULTICAST_IP "239.1.1.1"
#define PORT 8085

int main() {
    int sockfd;
    struct sockaddr_in local_addr;
    struct ip_mreq mreq;
    char buffer[1024];
    int reuse = 1;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

#ifdef SO_REUSEPORT
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse));
#endif

    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = INADDR_ANY;
    local_addr.sin_port = htons(PORT);

    bind(sockfd, (struct sockaddr *)&local_addr, sizeof(local_addr));

    mreq.imr_multiaddr.s_addr = inet_addr(MULTICAST_IP);
    mreq.imr_interface.s_addr = INADDR_ANY;

    setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));

    printf("Joined multicast group %s:%d\n", MULTICAST_IP, PORT);

    while (1) {
        recvfrom(sockfd, buffer, sizeof(buffer), 0, NULL, NULL);
        printf("Lecture: %s", buffer);
    }

    setsockopt(sockfd, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mreq, sizeof(mreq));

    close(sockfd);
    return 0;
}
