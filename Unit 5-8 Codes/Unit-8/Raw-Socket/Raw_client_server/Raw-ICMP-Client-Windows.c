#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")

struct icmp_header {
    unsigned char type;
    unsigned char code;
    unsigned short checksum;
    unsigned short id;
    unsigned short seq;
};

unsigned short calculate_checksum(unsigned short *data, int len) {
    unsigned long sum = 0;
    while (len > 1) {
        sum += *data++;
        len -= 2;
    }
    if (len) {
        sum += *(unsigned char *)data;
    }
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    return (unsigned short)~sum;
}

void error_exit(const char *msg) {
    printf("Error: %s, Code: %d\n", msg, WSAGetLastError());
    exit(1);
}

int main() {
    WSADATA wsa;
    SOCKET raw_socket;
    struct sockaddr_in dest;
    struct icmp_header icmp;
    char send_buffer[sizeof(struct icmp_header)];
    int send_size;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        error_exit("WSAStartup failed");
    }

    // Create raw socket
    raw_socket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (raw_socket == INVALID_SOCKET) {
        error_exit("Socket creation failed");
    }

    // Target destination
    dest.sin_family = AF_INET;
    dest.sin_addr.s_addr = inet_addr("8.8.8.8");  // Google's Public DNS

    // Fill ICMP header
    icmp.type = 8;  // Echo request
    icmp.code = 0;
    icmp.id = (unsigned short)GetCurrentProcessId();
    icmp.seq = 1;
    icmp.checksum = 0;
    icmp.checksum = calculate_checksum((unsigned short *)&icmp, sizeof(icmp));

    memcpy(send_buffer, &icmp, sizeof(icmp));
    send_size = sizeof(icmp);

    // Send the packet
    if (sendto(raw_socket, send_buffer, send_size, 0, (struct sockaddr *)&dest, sizeof(dest)) == SOCKET_ERROR) {
        error_exit("sendto failed");
    }

    printf("ICMP Echo Request sent to 8.8.8.8\n");

    closesocket(raw_socket);
    WSACleanup();
    return 0;
}
