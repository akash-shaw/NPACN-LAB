#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip_icmp.h>

#define DEST_IP "127.0.0.1"
#define ICMP_HDRLEN 8
#define PACKET_SIZE 64

// Function to calculate checksum
unsigned short checksum(void *b, int len) {
    unsigned short *buf = b;
    unsigned int sum = 0;
    unsigned short result;

    for (sum = 0; len > 1; len -= 2) {
        sum += *buf++;
    }

    if (len == 1) {
        sum += *(unsigned char *)buf;
    }

    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    
    return result;
}

int main() {
    int sockfd;
    struct sockaddr_in dest;
    char packet[PACKET_SIZE];
    struct icmphdr *icmp_hdr = (struct icmphdr *)packet;

    // Create raw socket
    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Fill ICMP Header
    memset(packet, 0, PACKET_SIZE);
    icmp_hdr->type = ICMP_ECHO;
    icmp_hdr->code = 0;
    icmp_hdr->un.echo.id = htons(getpid());
    icmp_hdr->un.echo.sequence = htons(1);
    icmp_hdr->checksum = 0;
    icmp_hdr->checksum = checksum(icmp_hdr, ICMP_HDRLEN);

    // Set destination address
    dest.sin_family = AF_INET;
    dest.sin_addr.s_addr = inet_addr(DEST_IP);

    // Send ICMP packet
    if (sendto(sockfd, packet, ICMP_HDRLEN, 0, (struct sockaddr *)&dest, sizeof(dest)) < 0) {
        perror("Packet send failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("ICMP Echo Request sent to %s\n", DEST_IP);

    close(sockfd);
    return 0;
}
