#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX 100

int main()
{
    int sockfd;
    struct sockaddr_in serveraddr;
    int n, arr[MAX];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) { perror("socket"); exit(1); }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(3388);
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if(connect(sockfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr)) < 0)
    {
        perror("connect");
        exit(1);
    }
    printf("Enter choice: ");
    printf("Enter number of elements: ");
    scanf("%d",&n);

    printf("Enter elements:\n");
    for(int i=0;i<n;i++)
        scanf("%d",&arr[i]);

    send(sockfd, &n, sizeof(int), 0);
    send(sockfd, arr, n * sizeof(int), 0);

    recv(sockfd, arr, n * sizeof(int), 0);

    printf("Sorted array:\n");
    for(int i=0;i<n;i++)
        printf("%d ",arr[i]);

    printf("\n");
    close(sockfd);
    return 0;
}
