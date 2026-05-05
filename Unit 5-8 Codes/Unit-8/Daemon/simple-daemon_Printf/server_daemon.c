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

#define MAXSIZE 90

void daemonize() {
    pid_t pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS); // Kill parent
    if (setsid() < 0) exit(EXIT_FAILURE); // Create new session
    
    umask(0);
    chdir("/");

    // Close standard file descriptors (daemons don't use terminal)
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

int main() {
    daemonize(); // The program now runs in the background

    int sockfd, newsockfd, retval;
    socklen_t actuallen;
    struct sockaddr_in serveraddr, clientaddr;
    char buff[MAXSIZE];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) exit(1);

    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(3380);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0) exit(1);
    if (listen(sockfd, 5) == -1) exit(1);

    while(1) { // Daemons usually run in a loop
        actuallen = sizeof(clientaddr);
        newsockfd = accept(sockfd, (struct sockaddr*)&clientaddr, &actuallen);
        
        if (newsockfd != -1) {
            recv(newsockfd, buff, sizeof(buff), 0);
            
            // Because it's a daemon, we can't use printf. 
            // In a real app, you'd write to a log file here.
            strcat(buff, " - Processed by Daemon"); 
            
            send(newsockfd, buff, sizeof(buff), 0);
            close(newsockfd);
        }
    }
    close(sockfd);
    return 0;
}