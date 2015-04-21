#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#define PORT 1234
void main()
{
	struct int_addr 
	{
		unsigned long s_addr;
	};
	
	struct sockaddr
	{
		short int sock_family;
		unsigned short int sock_port;
		struct int_addr inter_add;
		unsigned char sock_pad[8];
	};
	int sd;
	sd = socket(PF_INET,SOCK_STREAM,0);
	if(sd < 0)
	{
		printf("failed to obtain socket\n");
		exit(1);	
	}
	struct sockaddr serveraddr;
	serveraddr.sock_family = AF_INET;
	serveraddr.sock_port = htons(1234);
	serveraddr.inter_add.s_addr = inet_addr("127.0.0.1");
	memset(&serveraddr.sock_pad,'\0',sizeof(serveraddr.sock_pad));
	int status = -1;
	while(status < 0)
	{
		printf("trying %d\n",status);
		status = connect(sd,(struct sockaddr *)&serveraddr,sizeof(serveraddr));
	}
	if(status < 0)
	{
		printf("connection failed\n");
		exit(1);
	}
	char buff[] = "hello server";
	int res = send(sd,buff,10,0);
	printf("connection established.......\n");
	char *msg;
	msg = malloc(20*sizeof(char));
	int bytes;
	//char buffer[10101010101010101010] = "hello server";
//	bytes = send(sd,buffer,10,0);
//	bytes = send(sd,buffer,10,0);
	bytes = recv(sd,msg,20,0);
	printf("message from server: ");
	printf("%s\n",msg);
	free(msg);
	char *ms;
	ms = malloc(20*sizeof(char));
//	bytes = recv(sd,ms,20,0);
//	printf("message from server: %s\n",ms);
	close(sd);
	printf("connection terminated");
}
