#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<sys/types.h>

void errorrep(const char *reason)
{
	perror(reason);
	exit(1);
}

int creatsocket()
{
	printf("creating a socket\n");
	int i = socket(PF_INET,SOCK_DGRAM,0);
	if(i<0)
		perror("Failed to creat socket");
	return i;
}

void bindsocket(int socket,struct sockaddr_in addr)
{
	printf("binding the socket......\n");
	int res = bind(socket,(struct sockaddr *)&addr,sizeof(addr));
	if(res<0)
		perror("bind failed");
}


void main()
{
	//declaring sockaddresses
	struct sockaddr_in myaddr,saddr;

	//initialing sock addresses
	memset(&myaddr,0,sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_port = htons(1111);
	myaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	if(myaddr.sin_addr.s_addr == INADDR_NONE)
		errorrep("Invalid Address");

	//initializing server addr
	memset(&saddr,0,sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(1234);
	saddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	if(saddr.sin_addr.s_addr == INADDR_NONE)
		errorrep("Invalid Address server");

	//creating socket
	int sock = creatsocket();

	//binding the socket
	bindsocket(sock,myaddr);
	
	char command[20];
	char hostname[30];
	printf("entering shell exit shell to exit\n");
	while(1)
	{
		scanf("%s",&command[0]);
		scanf("%s",&hostname[0]);
		if(strcmp(&command[0],"exit") == 0)
		break;
		if(strcmp(&command[0],"nslookup") == 0)
		{
			char cbit;
			cbit = '1';
			sendto(sock,&cbit,1,0,(struct sockaddr *)&saddr,sizeof(saddr));
			sendto(sock,&hostname,30,0,(struct sockaddr *)&saddr,sizeof(saddr));
			char ip[30];
			int len = sizeof(saddr);
			recvfrom(sock,&ip,30,0,(struct sockaddr *)&saddr,&len);
			printf("%s\n",&ip[0]);
		}
	}
}
