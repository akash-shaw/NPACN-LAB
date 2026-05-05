#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <syslog.h>
#include <arpa/inet.h>
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

if (newsockfd < 0) {
    // Log as an ERROR if the connection fails
    syslog(LOG_ERR, "Accept failed: %m");
} else {
    memset(buffer, 0, MAXSIZE);
    int bytes = recv(newsockfd, buffer, MAXSIZE, 0);
    
    if (bytes > 0) {
        // Log as INFO for successful communication
        syslog(LOG_INFO, "Received from %s: %s", inet_ntoa(clientaddr.sin_addr), buffer);
        send(newsockfd, "ACK", 3, 0);
    } else {
        // Log as WARNING if the client disconnected unexpectedly
        syslog(LOG_WARNING, "Client %s disconnected without sending data", inet_ntoa(clientaddr.sin_addr));
    }
    close(newsockfd);
}
    }

    closelog();
    return 0;
}