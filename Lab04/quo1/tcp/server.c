#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#define PORT 9090
#define BUFFER_SIZE 1024

void handle_reg_no(int newsockfd, char *data) {
    char response[BUFFER_SIZE];
    // Mock Data
    sprintf(response, "Name: John Doe\nAddress: 123 Main St, New York\nChild PID: %d", getpid());
    write(newsockfd, response, strlen(response));
}

void handle_student_name(int newsockfd, char *data) {
    char response[BUFFER_SIZE];
    // Mock Data
    sprintf(response, "Dept: CS, Sem: 5, Sec: A\nCourses: CS101, CS102\nChild PID: %d", getpid());
    write(newsockfd, response, strlen(response));
}

void handle_subject_code(int newsockfd, char *data) {
    char response[BUFFER_SIZE];
    // Mock Data
    sprintf(response, "Subject: %s\nMarks Obtained: 85\nChild PID: %d", data, getpid());
    write(newsockfd, response, strlen(response));
}

int main() {
    int sockfd, newsockfd, clilen;
    struct sockaddr_in serv_addr, cli_addr;
    int option;
    char buffer[BUFFER_SIZE];

    // 1. Create Socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    // 2. Bind
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
        exit(1);
    }

    // 3. Listen
    listen(sockfd, 5);
    printf("TCP Server listening on port %d...\n", PORT);

    clilen = sizeof(cli_addr);

    while (1) {
        // 4. Accept Connection
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) {
            perror("ERROR on accept");
            continue;
        }

        // 5. Read Option and Data in Parent
        read(newsockfd, &option, sizeof(int));
        memset(buffer, 0, BUFFER_SIZE);
        read(newsockfd, buffer, BUFFER_SIZE); // Read the detail (RegNo/Name/Code)

        printf("Received Request: Option %d, Data: %s\n", option, buffer);

        // 6. Assign to Respective Child Process
        int pid = fork();

        if (pid < 0) {
            perror("ERROR on fork");
        } 
        else if (pid == 0) {
            // --- Child Process Logic ---
            close(sockfd); // Child doesn't need the listening socket

            if (option == 1) {
                handle_reg_no(newsockfd, buffer);
            } else if (option == 2) {
                handle_student_name(newsockfd, buffer);
            } else if (option == 3) {
                handle_subject_code(newsockfd, buffer);
            } else {
                char *msg = "Invalid Option";
                write(newsockfd, msg, strlen(msg));
            }
            
            close(newsockfd);
            exit(0); // Terminate child
        } 
        else {
            // --- Parent Process Logic ---
            close(newsockfd); // Parent closes client socket as child handles it
        }
    }
    close(sockfd);
    return 0;
}