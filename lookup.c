#include<stdio.h>
#include<unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

void main()
{
	printf("hostname:");
	char hostname[30];
	scanf("%s",&hostname[0]);
	struct hostent *hp;
	hp = gethostbyname(&hostname[0]);
	int x2;
	if(hp == NULL)
	{
		printf("error\n");
		exit(1);
	}
	printf("Host %s : \n" ,&hostname[0]);
	printf(" Officially:\t%s\n",hp->h_name);
	for ( x2=0; hp->h_addr_list[x2]; ++x2 )
	{
		printf(" Address:\t%s\n",inet_ntoa( *(struct in_addr *)hp->h_addr_list[x2]));
	}
}
