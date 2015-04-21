#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>

void errorrep(const char *reason)
{
	perror(reason);
	exit(1);
}

void main()
{
	int cfd,con_fd;
	FILE *fd;
	char *buffer;
	char *buf;
	buf = malloc(10*sizeof(char));
	buffer = malloc(256*sizeof(char));
	cfd = socket(PF_INET,SOCK_STREAM,0);
	if(cfd < 0)
		errorrep("socket");
	struct sockaddr_in serveraddr;
	memset(&serveraddr,0,sizeof(serveraddr));
	serveraddr.sin_family=AF_INET;
	serveraddr.sin_port=htons(1234);
	serveraddr.sin_addr.s_addr=inet_addr("127.0.0.1");
	con_fd = connect(cfd,&serveraddr,sizeof(serveraddr));
	if(con_fd < 0)
		errorrep("connect()");
	printf("enter file name: ");
	char *filename;
	filename = malloc(10*sizeof(char));
	scanf("%s",filename);
	fd = fopen(filename,"r+");
	if(fd == NULL)
		errorrep("file not found");
	while(1)
	{
		int read_bytes = fread(buffer,sizeof(char),sizeof(buffer),fd);
		if(read_bytes == 0)
			break;
		if(read_bytes < 0)
			printf("error reading from file\n");
		char *p = buffer;
		while(read_bytes > 0)
		{
			int bytes_sent = send(cfd,buffer,read_bytes,0);
			if(bytes_sent < 0)
			errorrep("send()");
			read_bytes = read_bytes - bytes_sent;
			p = p+bytes_sent;
		}
//			char ef = 'EOF';
//			int bytes_sent = send(cfd,&ef,sizeof(char),0);
//		int z = shutdown(cfd, 1);
//		if(z < 0)
//		errorrep("shutdown()");
//		printf("socket closed for write\n");
//		printf("socket closed for write\n");
//		printf("socket closed for write\n");
//		printf("socket closed for write\n");
//		printf("socket closed for write\n");
	}
	int z = shutdown(cfd, 1);
              if(z < 0)
              errorrep("shutdown()");
	printf("socket closed for write\n");
	printf("file sent for processing\n");
	printf("wating for server.........\n");
	int total=0;
	FILE *nf;
	nf = fopen("reverse","w+");
	if(nf == NULL)
	printf("unable to open file\n");
	while(1)
	{
		int rcv_bytes = recv(cfd,buf,1,0);
		if(rcv_bytes == 0)
		break;
		total= total + rcv_bytes;
//		printf("reveiving %d\n",rcv_bytes);
		fprintf(nf,"%s",buf);
	}
	close(cfd);
	fclose(nf);
	printf("received %d bytes",total);

}
