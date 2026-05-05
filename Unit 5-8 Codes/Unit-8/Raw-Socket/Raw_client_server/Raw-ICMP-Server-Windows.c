#include <stdio.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#define RECV_BUFSIZE 65536

void error_exit(const char *msg) {
    printf("Error: %s, Code: %d\n", msg, WSAGetLastError());
    exit(1);
}

int main() {
    WSADATA wsa;
    SOCKET raw_socket;
    struct sockaddr_in source;
    int source_size = sizeof(source);
    char buffer[RECV_BUFSIZE];

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        error_exit("WSAStartup failed");
    }

    // Create a raw socket to listen for ICMP packets
    raw_socket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (raw_socket == INVALID_SOCKET) {
        error_exit("Socket creation failed");
    }

    printf("Waiting for packets...\n");

    while (1) {
        int recv_size = recvfrom(raw_socket,
