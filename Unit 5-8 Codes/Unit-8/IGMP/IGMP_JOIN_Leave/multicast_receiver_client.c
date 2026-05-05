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
    int choice;
    int joined = 0;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = INADDR_ANY;
    local_addr.sin_port = htons(PORT);

    bind(sockfd, (struct sockaddr *)&local_addr, sizeof(local_addr));

    mreq.imr_multiaddr.s_addr = inet_addr(MULTICAST_IP);
    mreq.imr_interface.s_addr = INADDR_ANY;

    while (1) {
        printf("\n--- Multicast Client Menu ---\n");
        printf("1. Join Multicast Group\n");
        printf("2. Leave Multicast Group\n");
        printf("3. Receive Messages\n");
        printf("4. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        getchar();

        switch (choice) {
        case 1:
            if (!joined) {
                setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                           &mreq, sizeof(mreq));
                joined = 1;
                printf("Joined multicast group\n");
            } else {
                printf("Already a member\n");
            }
            break;

        case 2:
            if (joined) {
                setsockopt(sockfd, IPPROTO_IP, IP_DROP_MEMBERSHIP,
                           &mreq, sizeof(mreq));
                joined = 0;
                printf("Left multicast group\n");
            } else {
                printf("Not a member yet\n");
            }
            break;

        case 3:
            if (!joined) {
                printf("Join the group first!\n");
                break;
            }
            printf("Waiting for multicast messages (Ctrl+C to stop)...\n");
            recvfrom(sockfd, buffer, sizeof(buffer), 0, NULL, NULL);
            printf("Lecture: %s\n", buffer);
            break;

        case 4:
            if (joined) {
                setsockopt(sockfd, IPPROTO_IP, IP_DROP_MEMBERSHIP,
                           &mreq, sizeof(mreq));
            }
            close(sockfd);
            exit(0);

        default:
            printf("Invalid choice\n");
        }
    }
}
