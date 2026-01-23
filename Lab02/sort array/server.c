#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX 100

void sort(int a[], int n)
{
    for(int i=0;i<n-1;i++)
        for(int j=i+1;j<n;j++)
            if(a[i] > a[j])
            {
                int t=a[i];
                a[i]=a[j];
                a[j]=t;
            }
}

int main()
{
    int sockfd, newsockfd;
    struct sockaddr_in serveraddr, clientaddr;
    socklen_t len = sizeof(clientaddr);

    int n, arr[MAX];


    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) { perror("socket"); exit(1); }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(3388);
    serveraddr.sin_addr.s_addr = INADDR_ANY;

    if(bind(sockfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr)) < 0)
    {
        perror("bind");
        exit(1);
    }

    listen(sockfd,1);
    printf("Server waiting...\n");

    newsockfd = accept(sockfd,(struct sockaddr*)&clientaddr,&len);
    if(newsockfd < 0) { perror("accept"); exit(1); }

    recv(newsockfd, &n, sizeof(int), 0);
    recv(newsockfd, arr, n * sizeof(int), 0);

    sort(arr, n);

    send(newsockfd, arr, n * sizeof(int), 0);

    close(newsockfd);
    close(sockfd);
    return 0;
}
