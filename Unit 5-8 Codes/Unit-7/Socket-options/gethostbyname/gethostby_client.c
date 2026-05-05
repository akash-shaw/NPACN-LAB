#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<errno.h>

#define MAXSIZE 90

void main()
{
    int sockfd, dnsfd;
    struct sockaddr_in serveraddr, dnsaddr;
    char buff[MAXSIZE];
    struct hostent *he;
    struct addrinfo hints, *res;

    /* gethostbyname */
    he = gethostbyname("localhost");
    if(he)
        printf("gethostbyname: %s\n",
               inet_ntoa(*(struct in_addr*)he->h_addr));

    /* gethostbyname2 */
    he = gethostbyname2("localhost", AF_INET);
    if(he)
        printf("gethostbyname2: %s\n",
               inet_ntoa(*(struct in_addr*)he->h_addr));

    /* getaddrinfo */
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    getaddrinfo("localhost", NULL, &hints, &res);

    struct sockaddr_in *addr = (struct sockaddr_in*)res->ai_addr;
    printf("getaddrinfo: %s\n", inet_ntoa(addr->sin_addr));
    freeaddrinfo(res);

    /* Query application-level DNS server */
    dnsfd = socket(AF_INET, SOCK_STREAM, 0);
    dnsaddr.sin_family = AF_INET;
    dnsaddr.sin_port = htons(5353);
    dnsaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(dnsfd, (struct sockaddr*)&dnsaddr, sizeof(dnsaddr));
    strcpy(buff, "example.com");
    send(dnsfd, buff, sizeof(buff), 0);
    recv(dnsfd, buff, sizeof(buff), 0);

    printf("DNS Server Response: %s\n", buff);
    close(dnsfd);

    /* Connect to main server */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(3380);
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    strcpy(buff, "Hello from Client");
    send(sockfd, buff, sizeof(buff), 0);

    recv(sockfd, buff, sizeof(buff), 0);
    puts(buff);

    close(sockfd);
}
