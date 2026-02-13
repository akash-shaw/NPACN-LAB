#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <ctype.h>

#define PORT 8080
#define BUFFER_SIZE 2048

// Helper function to check if file exists
int file_exists(const char *filename) {
    return access(filename, F_OK) != -1;
}

// Helper to count occurrences of a string in the file
int search_string(char *filename, char *str) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return -1;
    
    char temp[BUFFER_SIZE];
    int count = 0;
    int str_len = strlen(str);
    
    while (fgets(temp, BUFFER_SIZE, fp) != NULL) {
        char *ptr = temp;
        while ((ptr = strstr(ptr, str)) != NULL) {
            count++;
            ptr += str_len;
        }
    }
    fclose(fp);
    return count;
}

// Helper to replace string in file
int replace_string(char *filename, char *str1, char *str2) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return 0;
    
    // Read entire file into buffer
    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    char *content = malloc(fsize + 1);
    fread(content, 1, fsize, fp);
    content[fsize] = '\0';
    fclose(fp);
    
    // Check if str1 exists
    if (strstr(content, str1) == NULL) {
        free(content);
        return 0; // Not found
    }
    
    // Create new buffer for replacement
    // Note: In a real scenario, we'd calculate exact new size. 
    // Here allocating enough space safely.
    char *new_content = malloc(fsize * 2 + 1000); 
    char *pos, *temp = content;
    char *current = new_content;
    int str1_len = strlen(str1);
    int str2_len = strlen(str2);
    
    while ((pos = strstr(temp, str1)) != NULL) {
        int len = pos - temp;
        strncpy(current, temp, len);
        current += len;
        strcpy(current, str2);
        current += str2_len;
        temp = pos + str1_len;
    }
    strcpy(current, temp); // Copy remaining
    
    // Write back to file
    fp = fopen(filename, "w");
    fprintf(fp, "%s", new_content);
    fclose(fp);
    
    free(content);
    free(new_content);
    return 1; // Replaced
}

// Helper to reorder file content by ASCII
void reorder_file(char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return;
    
    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    char *content = malloc(fsize);
    fread(content, 1, fsize, fp);
    fclose(fp);
    
    // Bubble sort characters (simple implementation)
    for (long i = 0; i < fsize - 1; i++) {
        for (long j = 0; j < fsize - i - 1; j++) {
            if (content[j] > content[j + 1]) {
                char temp = content[j];
                content[j] = content[j + 1];
                content[j + 1] = temp;
            }
        }
    }
    
    fp = fopen(filename, "w");
    fwrite(content, 1, fsize, fp);
    fclose(fp);
    free(content);
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    char filename[100];

    // 1. Create Socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // 2. Bind
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // 3. Listen
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    
    printf("TCP Server waiting for client...\n");

    // 4. Accept
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    // Receive Filename
    read(new_socket, filename, 100);
    printf("Client requested file: %s\n", filename);

    if (!file_exists(filename)) {
        char *msg = "File not present";
        send(new_socket, msg, strlen(msg), 0);
        printf("File not found. Terminating.\n");
        close(new_socket);
        close(server_fd);
        return 0;
    } else {
        char *msg = "File present";
        send(new_socket, msg, strlen(msg), 0);
    }

    // Menu Loop
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int valread = read(new_socket, buffer, BUFFER_SIZE);
        if (valread <= 0) break;

        int choice = buffer[0] - '0'; // Extract choice
        char response[BUFFER_SIZE] = {0};

        if (choice == 1) { // Search
            char *search_str = buffer + 2; // Skip "1 "
            int count = search_string(filename, search_str);
            if (count > 0)
                sprintf(response, "String occurred %d times", count);
            else
                strcpy(response, "String not found");
                
        } else if (choice == 2) { // Replace
            // Format received: "2 str1 str2"
            char str1[100], str2[100];
            sscanf(buffer + 2, "%s %s", str1, str2);
            
            if (replace_string(filename, str1, str2))
                strcpy(response, "String replaced");
            else
                strcpy(response, "String not found");

        } else if (choice == 3) { // Reorder
            reorder_file(filename);
            strcpy(response, "File reordered successfully");

        } else if (choice == 4) { // Exit
            printf("Client requested exit.\n");
            break;
        }

        send(new_socket, response, strlen(response), 0);
    }

    close(new_socket);
    close(server_fd);
    return 0;
}