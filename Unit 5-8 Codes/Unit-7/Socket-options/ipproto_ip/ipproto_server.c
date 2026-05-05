#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<errno.h>

#define MAXSIZE 90

int main()
{
    int sockfd, retval;
    struct sockaddr_in serveraddr, clientaddr;
    socklen_t addrlen;

    char databuf[MAXSIZE];
    char cmsgbuf[256];

    struct iovec iov;
    struct msghdr msg;
    struct cmsghdr *cmsg;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0)
    {
        perror("Socket creation error");
        exit(1);
    }

    /* Enable socket options */
    int on = 1;
   // setsockopt(sockfd, IPPROTO_IP, IP_RECVDSTADDR, &on, sizeof(on));
    setsockopt(sockfd, IPPROTO_IP, IP_PKTINFO, &on, sizeof(on));
    setsockopt(sockfd, IPPROTO_IP, IP_RECVTOS, &on, sizeof(on));
    setsockopt(sockfd, IPPROTO_IP, IP_RECVTTL, &on, sizeof(on));

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(3380);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    retval = bind(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if(retval < 0)
    {
        perror("Bind error");
        close(sockfd);
        exit(1);
    }

    memset(&msg, 0, sizeof(msg));
    memset(databuf, 0, MAXSIZE);
    memset(cmsgbuf, 0, sizeof(cmsgbuf));

    iov.iov_base = databuf;
    iov.iov_len = MAXSIZE;

    msg.msg_name = &clientaddr;
    msg.msg_namelen = sizeof(clientaddr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_control = cmsgbuf;
    msg.msg_controllen = sizeof(cmsgbuf);

    recvmsg(sockfd, &msg, 0);

    printf("Received message: %s\n", databuf);

    /* Parse ancillary data */
    for(cmsg = CMSG_FIRSTHDR(&msg);
        cmsg != NULL;
        cmsg = CMSG_NXTHDR(&msg, cmsg))
    {
        if(cmsg->cmsg_level == IPPROTO_IP)
        {
          /*  if(cmsg->cmsg_type == IP_RECVDSTADDR)
            {
                struct in_addr *dst =
                    (struct in_addr *)CMSG_DATA(cmsg);
                printf("Destination IP: %s\n", inet_ntoa(*dst));
            }
*/
            if(cmsg->cmsg_type == IP_PKTINFO)
            {
                struct in_pktinfo *pkt =
                    (struct in_pktinfo *)CMSG_DATA(cmsg);
                printf("Incoming Interface Index: %d\n", pkt->ipi_ifindex);
                printf("Destination IP (pktinfo): %s\n",
                       inet_ntoa(pkt->ipi_addr));
            }

            if(cmsg->cmsg_type == IP_TOS)
            {
                int tos = *(int *)CMSG_DATA(cmsg);
                printf("IP TOS: %d\n", tos);
            }

            if(cmsg->cmsg_type == IP_TTL)
            {
                int ttl = *(int *)CMSG_DATA(cmsg);
                printf("IP TTL: %d\n", ttl);
            }
        }
    }

    close(sockfd);
    return 0;
}
