#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<arpa/inet.h>
void errorrep(const char *reason)
{
//	printf("%s", strerror(errno));
	perror(reason);
	exit(1);
}

void main()
{
//	close(sf);
	int sf;
	close(sf);
	sf = socket(PF_INET,SOCK_STREAM,0);
	char *buffer;
	buffer = malloc(10000*sizeof(char));
	if(sf < 0)
	errorrep("socket()");
	struct sockaddr_in sockadr, theiradr;
	memset(&sockadr,0,sizeof(sockadr));
	sockadr.sin_family = AF_INET;
	sockadr.sin_port = htons(1234);
	sockadr.sin_addr.s_addr = INADDR_ANY;
	int bind_res = bind(sf,(struct sockaddr *)&sockadr,sizeof sockadr);
	if(bind_res < 0)
	errorrep("bind()");
//	system("netstat -tapen | grep 1234");
	int lis_status = listen(sf,3);
	if(lis_status < 0)
	errorrep("listen()");
	printf("litstening of port 1234\n");
//	system("netstat -tapen | grep 1234");
	int sin_size = sizeof(struct sockaddr_in);
	int accept_fd = accept(sf,&theiradr,&sin_size);
	if(accept_fd < 0)
	errorrep("accept()");
	printf("connected\n");
	int total = 0;
	FILE *nf;
	nf = fopen("temp","w+");
	if(nf == NULL)
	printf("unable to open file");
	int rcv_bytes;
	while(1)
	{
		buffer = malloc(10000*sizeof(char));
//		printf("entered while\n");
		int rcv_bytes = recv(accept_fd,buffer,10000,0);
		if(rcv_bytes <= 0 )
		{
			printf("entering break");
			break;
		}
		total = total + rcv_bytes;
		int e;
//		for(e=0;e<10000;e++)
//		*(buffer + e) = '';
//		free(buffer);
		printf("receiving %d\n",rcv_bytes);
		printf("%s",buffer);
//		bzero(buffer,256);
		free(buffer);
	}
/*	printf("received %d bytes\n",total);
	printf("reversing system engaged\n");
	fseek(nf,-2L,SEEK_CUR);
	printf("\n%d\n",total);
	while(total != 1)
	{
//		printf("stuck here");
//		fseek(nf,-2L,SEEK_CUR);
		char temp = fgetc(nf);
		fseek(nf,-2L,SEEK_CUR);
		printf("%c",temp);
		int send_bytes = send(accept_fd,&temp,1,0);
		if(send_bytes < 0)
		errorrep("send()");
		total--;	
	}
	int z = shutdown(accept_fd,1);
	if(z < 0)
	errorrep("shutdown");
	char test = fgetc(nf);
	printf("%c",test);*/
	close(sf);
	close(accept_fd);
}
