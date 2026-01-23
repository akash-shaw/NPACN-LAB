#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    char input_string[BUFFER_SIZE];

    // 1. Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    // 2. Connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    // 3. Get input from user
    printf("At the client side:\n");
    printf("Input string: ");
    scanf("%s", input_string);

    // 4. Send string to server
    send(sock, input_string, strlen(input_string), 0);

    // 5. Read response from server
    // We read in a loop because the server sends data in two separate chunks (Child then Parent)
    printf("\nAt the server side:\n");
    int valread;
    while ((valread = read(sock, buffer, BUFFER_SIZE)) > 0) {
        // Print whatever we receive (null terminate just in case)
        buffer[valread] = '\0';
        printf("%s", buffer);
        
        // Reset buffer for next read
        memset(buffer, 0, BUFFER_SIZE);
    }

    close(sock);
    return 0;
}