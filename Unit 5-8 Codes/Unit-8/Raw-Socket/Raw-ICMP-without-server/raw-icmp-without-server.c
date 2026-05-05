#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <sys/time.h>
#include <errno.h>

#define DEST_IP "127.0.0.1"
#define PACKET_SIZE 64

/* Checksum calculation */
unsigned short checksum(void *data, int len) {
    unsigned short *buf = data;
    unsigned int sum = 0;

    while (len > 1) {
        sum += *buf++;
        len -= 2;
    }

    if (len == 1)
        sum += *(unsigned char *)buf;

    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);

    return (unsigned short)(~sum);
}

int main() {
    int sockfd;
    struct sockaddr_in dest_addr;
    char packet[PACKET_SIZE];
    struct icmphdr *icmp;
    int seq = 1;

    /* Create RAW socket */
    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        perror("socket");
        exit(1);
    }

    /* Set receive timeout */
    struct timeval timeout = {1, 0};
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,
               &timeout, sizeof(timeout));

    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = inet_addr(DEST_IP);

    while (1) {
        memset(packet, 0, PACKET_SIZE);

        /* ICMP header */
        icmp = (struct icmphdr *)packet;
        icmp->type = ICMP_ECHO;
        icmp->code = 0;
        icmp->un.echo.id = getpid();
        icmp->un.echo.sequence = seq++;
        icmp->checksum = 0;

        /* Payload */
        strcpy(packet + sizeof(struct icmphdr),
               "RAW SOCKET ICMP DEMO");

        /* Checksum */
        icmp->checksum = checksum(packet, PACKET_SIZE);

        /* Send Echo Request */
        sendto(sockfd, packet, PACKET_SIZE, 0,
               (struct sockaddr *)&dest_addr,
               sizeof(dest_addr));

        /* Receive Echo Reply */
        if (recvfrom(sockfd, packet, PACKET_SIZE, 0,
                     NULL, NULL) < 0) {
            printf("Request timeout\n");
        } else {
            printf("Reply from %s: icmp_seq=%d\n",
                   DEST_IP, icmp->un.echo.sequence);
        }

        sleep(1);
    }

    close(sockfd);
    return 0;
}
