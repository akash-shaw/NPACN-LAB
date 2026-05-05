#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip_icmp.h>

#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in source;
    char buffer[BUFFER_SIZE];

    // Create raw socket to capture ICMP packets
    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    printf("Raw socket created. Listening for ICMP packets...\n");

    while (1) {
        socklen_t addr_len = sizeof(source);
        ssize_t data_size = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&source, &addr_len);
        if (data_size < 0) {
            perror("recvfrom failed");
            close(sockfd);
            exit(EXIT_FAILURE);
        }

        struct icmphdr *icmp = (struct icmphdr *)(buffer + sizeof(struct iphdr));

        printf("Received ICMP packet from %s, type: %d, code: %d, size: %ld bytes\n",
               inet_ntoa(source.sin_addr), icmp->type, icmp->code, data_size);
    }

    close(sockfd);
    return 0;
}
