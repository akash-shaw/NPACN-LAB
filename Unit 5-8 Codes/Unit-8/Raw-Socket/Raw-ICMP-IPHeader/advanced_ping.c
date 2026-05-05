#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <sys/time.h>
#include <sys/select.h>

#define PACKET_SIZE 84
#define PING_COUNT 4

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

int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("Usage: sudo %s <destination_ip>\n", argv[0]);
        return 1;
    }

    int sockfd;
    struct sockaddr_in dest_addr;
    char packet[PACKET_SIZE];
    int sent = 0, received = 0;

    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        perror("socket");
        return 1;
    }

    int opt = 1;
    setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &opt, sizeof(opt));

    dest_addr.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &dest_addr.sin_addr);

    printf("PING %s with RAW socket:\n\n", argv[1]);

    for (int seq = 1; seq <= PING_COUNT; seq++) {

        memset(packet, 0, PACKET_SIZE);

        struct iphdr *ip = (struct iphdr *)packet;
        struct icmphdr *icmp = (struct icmphdr *)(packet + sizeof(struct iphdr));

        /* Build IP header */
        ip->ihl = 5;
        ip->version = 4;
        ip->tos = 0;
        ip->tot_len = htons(PACKET_SIZE);
        ip->id = htons(seq);
        ip->ttl = 64;
        ip->protocol = IPPROTO_ICMP;
        ip->saddr = inet_addr("0.0.0.0");  // Kernel fills source
        ip->daddr = dest_addr.sin_addr.s_addr;
        ip->check = checksum(ip, sizeof(struct iphdr));

        /* Build ICMP header */
        icmp->type = ICMP_ECHO;
        icmp->code = 0;
        icmp->un.echo.id = getpid();
        icmp->un.echo.sequence = seq;

        strcpy(packet + sizeof(struct iphdr) + sizeof(struct icmphdr),
               "ADVANCED RAW ICMP");

        icmp->checksum = checksum(icmp,
                         PACKET_SIZE - sizeof(struct iphdr));

        struct timeval start, end;
        gettimeofday(&start, NULL);

        sendto(sockfd, packet, PACKET_SIZE, 0,
               (struct sockaddr *)&dest_addr,
               sizeof(dest_addr));

        sent++;

        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);

        struct timeval timeout = {1, 0};

        if (select(sockfd + 1, &readfds, NULL, NULL, &timeout) > 0) {

            recvfrom(sockfd, packet, PACKET_SIZE, 0, NULL, NULL);
            gettimeofday(&end, NULL);

            double rtt = (end.tv_sec - start.tv_sec) * 1000.0 +
                         (end.tv_usec - start.tv_usec) / 1000.0;

            printf("Reply from %s: icmp_seq=%d ttl=%d time=%.2f ms\n",
                   argv[1], seq, ip->ttl, rtt);

            received++;
        } else {
            printf("Request timeout for icmp_seq %d\n", seq);
        }

        sleep(1);
    }

    printf("\n--- Ping Statistics ---\n");
    printf("%d packets transmitted, %d received, %.0f%% packet loss\n",
           sent, received,
           ((sent - received) * 100.0) / sent);

    close(sockfd);
    return 0;
}
