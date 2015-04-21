#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/sendfile.h>
void errorrep(const char *reason)
{
	perror(reason);
	exit(1);
}

void sendf(char *file,int cfd,struct sockaddr_in serveraddr)
{
	char *cmdbit;
	cmdbit = "1";
	sendto(cfd,cmdbit,1,0,(struct sockaddr *)&serveraddr,sizeof(serveraddr));
	int fd;
	sendto(cfd,file,10,0,(struct sockaddr *)&serveraddr,sizeof(serveraddr));
	char size[10];
	fd = open(file,O_CREAT);
	if(fd < 0)
	printf("unable to open file\n");
	off_t offset = 0;
	struct stat stat_buf;
	fstat(fd, &stat_buf);
	itoa(stat_buf.st_size,&size[0],10);
        sendto(cfd,&size[0],10,0,(struct sockaddr *)&serveraddr,sizeof(serveraddr));
	int rc = sendfile (cfd, fd, &offset, stat_buf.st_size);
	if(rc < 0)
	errorrep("sendfile()");
	close(fd);
}
void getfile(char *file)
{

}

void main()
{
	int cfd,con_fd;
	//int fd;
	char *buffer;
	char *buf;

	buf = malloc(10*sizeof(char));
	buffer = malloc(256*sizeof(char));
	cfd = socket(PF_INET,SOCK_DGRAM,0);

	if(cfd < 0)
		errorrep("socket");

	struct sockaddr_in serveraddr;
	memset((struct sockaddr *)&serveraddr,0,sizeof(serveraddr));
	serveraddr.sin_family=AF_INET;
	serveraddr.sin_port=htons(1234);
	serveraddr.sin_addr.s_addr=inet_addr("127.0.0.1");
//	con_fd = connect(cfd,&serveraddr,sizeof(serveraddr));

	struct sockaddr_in sockadr,temp;
        memset(&sockadr,0,sizeof(sockadr));
        sockadr.sin_family = AF_INET;
        sockadr.sin_port = htons(1111);
        sockadr.sin_addr.s_addr =inet_addr("127.0.0.1");
        int bind_res = bind(cfd,(struct sockaddr *)&sockadr,sizeof sockadr);
        if(bind_res < 0)
        errorrep("bind()");



	printf("username:");
	char *username,*passwd;
	username = malloc(10*sizeof(char));
	passwd = malloc(10*sizeof(char));
	scanf("%s",username);
	printf("passwd:");
	scanf("%s",passwd);
	printf("sending authreq\n");
	int s_res = sendto(cfd,username,10,0,(struct sockaddr *)&serveraddr,sizeof(serveraddr));
	if(s_res < 0)
	errorrep("sendto()");
	s_res = sendto(cfd,passwd,10,0,(struct sockaddr *)&serveraddr,sizeof(serveraddr));
	if(s_res < 0)
	errorrep("sendto() passwd");
	char *msg;
	msg = malloc(20*sizeof(char));
	int length = sizeof(sockadr);
	int r_res = recvfrom(cfd,msg,20,0,(struct sockaddr *)&sockadr,&length);
	int login = strcmp(msg,"login sucess");
	printf("%s\n",msg);	
	if(login == 0)
	{
		printf("exit to end the program\n");
		char command[6];
		char filename[10];
		while(1)
		{
			scanf("%s",&command[0]);
			scanf("%s",&filename[0]);
			if(strcmp(&command[0],"put") == 0)
			{
				sendf(&filename[0],cfd,serveraddr);
			}
			if(strcmp(&command[0],"get") == 0)
			{
				getfile(&filename[0]);
			}
			if(strcmp(&command[0],"exit"))
			{
				close(cfd);
				exit(1);
			}
			else
				printf("invalid command\n");
				printf("use exit command to exit the program\n");
			
		}
		
	}
	
	
/*	char *filename;
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
	printf("received %d bytes",total);*/
	close(cfd);

}
