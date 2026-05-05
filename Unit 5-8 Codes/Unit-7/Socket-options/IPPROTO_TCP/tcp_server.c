#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netinet/tcp.h>
#include <errno.h>

#define MAXSIZE 90

void main()
{
    int sockfd, newsockfd, retval;
    socklen_t actuallen;
    int recedbytes, sentbytes;
    struct sockaddr_in serveraddr, clientaddr;
    char buff[MAXSIZE];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1)
    {
        printf("Socket creation error\n");
        exit(1);
    }

    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    /* Enable TCP Keepalive */
    setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt));

    int idle = 60;      // seconds before keepalive probes
    int interval = 10;  // interval between probes
    int count = 5;      // number of probes

    setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPIDLE, &idle, sizeof(idle));
    setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPINTVL, &interval, sizeof(interval));
    setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPCNT, &count, sizeof(count));

    /* Disable Nagle's algorithm */
    setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));

    /* Set Maximum Segment Size */
    int mss = 1460;
    setsockopt(sockfd, IPPROTO_TCP, TCP_MAXSEG, &mss, sizeof(mss));


    /* TCP_MAXRT is NOT supported on Linux */
    /*
    int maxrt = 30;
    setsockopt(sockfd, IPPROTO_TCP, TCP_MAXRT, &maxrt, sizeof(maxrt));
    */

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(3380);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    retval = bind(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if(retval < 0)
    {
        printf("Binding error: %d\n", errno);
        close(sockfd);
        exit(1);
    }

    listen(sockfd, 1);

    actuallen = sizeof(clientaddr);
    newsockfd = accept(sockfd, (struct sockaddr*)&clientaddr, &actuallen);
    /* ---- VERIFY TCP OPTIONS USING getsockopt() ---- */

int optval;
socklen_t optlen = sizeof(optval);

/* SO_KEEPALIVE */
getsockopt(newsockfd, SOL_SOCKET, SO_KEEPALIVE, &optval, &optlen);
printf("SO_KEEPALIVE = %d\n", optval);

/* TCP_NODELAY */
getsockopt(newsockfd, IPPROTO_TCP, TCP_NODELAY, &optval, &optlen);
printf("TCP_NODELAY = %d\n", optval);

/* TCP_MAXSEG */
getsockopt(newsockfd, IPPROTO_TCP, TCP_MAXSEG, &optval, &optlen);
printf("TCP_MAXSEG = %d bytes\n", optval);

/* TCP_KEEPIDLE */
getsockopt(newsockfd, IPPROTO_TCP, TCP_KEEPIDLE, &optval, &optlen);
printf("TCP_KEEPIDLE = %d seconds\n", optval);

/* TCP_KEEPINTVL */
getsockopt(newsockfd, IPPROTO_TCP, TCP_KEEPINTVL, &optval, &optlen);
printf("TCP_KEEPINTVL = %d seconds\n", optval);

/* TCP_KEEPCNT */
getsockopt(newsockfd, IPPROTO_TCP, TCP_KEEPCNT, &optval, &optlen);
printf("TCP_KEEPCNT = %d\n", optval);
    
    
    if(newsockfd < 0)
    {
        close(sockfd);
        exit(1);
    }

    recedbytes = recv(newsockfd, buff, sizeof(buff), 0);
    if(recedbytes < 0)
    {
        close(sockfd);
        close(newsockfd);
        exit(1);
    }

    puts(buff);

    fgets(buff, MAXSIZE, stdin);
    sentbytes = send(newsockfd, buff, strlen(buff)+1, 0);

    close(newsockfd);
    close(sockfd);
}
