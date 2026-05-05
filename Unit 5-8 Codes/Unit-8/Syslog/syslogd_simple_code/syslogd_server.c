#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <syslog.h>
#include <sys/stat.h>

#define PORT 3380
#define MAXSIZE 1024

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
    // 1. Start the Daemon
    daemonize();

    // 2. Initialize Syslog
    // LOG_PID: Include the process ID in each message
    // LOG_DAEMON: Categorize this as a system daemon message
    openlog("MySyslogDaemon", LOG_PID, LOG_DAEMON);
    syslog(LOG_INFO, "Daemon started and listening on port %d", PORT);

    int sockfd, newsockfd;
    struct sockaddr_in serveraddr, clientaddr;
    socklen_t addrlen = sizeof(clientaddr);
    char buffer[MAXSIZE];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = INADDR_ANY;
    serveraddr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0) {
        syslog(LOG_ERR, "Binding failed: %m"); // %m prints errno automatically
        exit(EXIT_FAILURE);
    }

    listen(sockfd, 5);

    while (1) {
        newsockfd = accept(sockfd, (struct sockaddr *)&clientaddr, &addrlen);
        if (newsockfd >= 0) {
            memset(buffer, 0, MAXSIZE);
            recv(newsockfd, buffer, MAXSIZE, 0);
            
            // Log the received message to Syslog
            syslog(LOG_NOTICE, "Client sent: %s", buffer);
            
            send(newsockfd, "Message Received by Syslogd", 27, 0);
            close(newsockfd);
        }
    }

    closelog();
    return 0;
}