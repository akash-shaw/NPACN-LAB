#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
//#include<sys/socket.h>
#include<winsock2.h>
#include<sys/types.h>
//#include<netinet/in.h>
int  sockfd_to_family(int sockfd)
{
	union {
	  struct sockaddr	sa;
	  char	data[MAXSOCKADDR];
	} un;
	socklen_t	len;

	len = MAXSOCKADDR;
	if (getsockname(sockfd, (sa *) un.data, &len) < 0)
		return(-1);
	return(un.sa.sa_family);
}
