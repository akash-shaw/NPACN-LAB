#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/stat.h>
#include <time.h>

#define MAXSIZE 90
#define LOG_FILE "/tmp/daemon.log"

void log_message(const char* message) {
    // Use a path inside WSL (like /tmp or /home) for better performance
    FILE *f = fopen("/tmp/daemon.log", "a"); 
    if (f == NULL) return;
    
    time_t now = time(NULL);
    char *timestamp = ctime(&now);
    timestamp[strlen(timestamp) - 1] = '\0'; 
    
    fprintf(f, "[%s] %s\n", timestamp, message);
    
    fflush(f);  // <--- CRITICAL: This sends data to the file immediately
    fclose(f);
}

void daemonize() {
    pid_t pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS); 
    if (setsid() < 0) exit(EXIT_FAILURE); 
    
    umask(0);
    chdir("/");

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

int main() {
   daemonize();
    log_message("Daemon started successfully.");

    int sockfd, newsockfd;
    socklen_t actuallen;
    struct sockaddr_in serveraddr, clientaddr;
    char buff[MAXSIZE];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(3380);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    listen(sockfd, 5);

    while(1) {
        actuallen = sizeof(clientaddr);
        newsockfd = accept(sockfd, (struct sockaddr*)&clientaddr, &actuallen);
        
        if (newsockfd != -1) {
            memset(buff, 0, MAXSIZE);
            recv(newsockfd, buff, sizeof(buff), 0);
            printf("hello\n");
puts(buff);
            // Log the client's IP and message
            char log_buf[128];
            snprintf(log_buf, sizeof(log_buf), "Connection from %s: %s", 
                     inet_ntoa(clientaddr.sin_addr), buff);
            log_message(log_buf);
            
            strcat(buff, " (ACK by Daemon)"); 
            send(newsockfd, buff, sizeof(buff), 0);
            close(newsockfd);
        }
        else{
            printf("hello\n");
        }
    }
    return 0;
}