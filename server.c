#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<stdlib.h>
//#define MYPORT 1234
void main()
{
	short MYPORT= 1234;
	int socketd,new_fd;


	struct in_addr {
		unsigned long s_addr;
	};

	struct sockaddr_in {
		short int sin_family;
		unsigned short int sin_port;
		struct in_addr sin_addr;
		unsigned char sin_zero[8];
	};

	printf("getting a socket\n");

	socketd = socket(PF_INET,SOCK_STREAM,0);
	if (socketd < 0)
	{
		printf("socket discriptor failed\n");
		exit(1);
	}
	struct sockaddr_in my_addr;
	my_addr.sin_family=AF_INET;
	my_addr.sin_port=htons(MYPORT);
	my_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
	memset(&my_addr.sin_zero,'\0',sizeof(my_addr.sin_zero));

	printf("%s",&(my_addr.sin_addr.s_addr));
	printf("binding on port 1234\n");
	int status = bind(socketd,(struct sockaddr *)&my_addr,sizeof(my_addr));
	if(status < 0)
	{
		printf("bind failed\n");
		exit(1);
	}	
	status = listen(socketd,3);
	printf("listening on port 1111\n");
	if(status < 0 )
	{
		printf("error\n");
		exit(1);
	}
	struct sockaddr their_addr;
	socklen_t addr_size;
	new_fd = -1;
	new_fd = accept(socketd,&their_addr,&addr_size);
	if(new_fd != -1)

{	char *msg = "Hello World";
	int bytessent = send(new_fd,msg,11,0);
	bytessent = send(new_fd,msg,11,0);
	char he[10];
	bytessent = recv(new_fd,he,10,0);
	printf("message from client:%s\n",he);
//	bytessent = recv(new_fd,m,10,0);
	close(new_fd);
}
}
