#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<string.h>
void errorrep(const char *reason)
{
//	printf("%s", strerror(errno));
	perror(reason);
	exit(1);
}
void rfile(int sf)
{
	struct sockaddr_in theiradr;
	int te = sizeof(theiradr);
	theiradr.sin_family = AF_INET;
        theiradr.sin_port = htons(1111);
        theiradr.sin_addr.s_addr=inet_addr("127.0.0.1");
        if ( theiradr.sin_addr.s_addr == INADDR_NONE )
        errorrep("bad address.");

	//receiving metadata
	char filesize[10];

	//receiving file size
	recvfrom(sf,&filesize[0],10,0,(struct sockaddr *)&theiradr,&te);
	int file_size=atoi(&filesize[0]);

	//receiving file name
	char filename[10];
	recvfrom(sf,&filename[0],10,0,(struct sockaddr *)&theiradr,&te);
		
	//creating file
	FILE *newfile;
	newfile = fopen(&filename[0],"w+");
		
	char *buffer;
	buffer = malloc(file_size*sizeof(char));
	//receiving contents of the file
        int bytes_rcv = 0;
	while(bytes_rcv < file_size)
        {
		int t = recvfrom(sf,buffer,file_size,0,(struct sockaddr *)&theiradr,&te);
		bytes_rcv = bytes_rcv + t;
		fprintf(newfile,"%s",buffer);
	}
	fclose(newfile);


}
void main()
{
	int sf;
	close(sf);
	sf = socket(PF_INET,SOCK_DGRAM,0);
	char *buffer;
	buffer = malloc(256*sizeof(char));
	if(sf < 0)
	errorrep("socket()");
	struct sockaddr_in sockadr, theiradr,temp,temp1;
	memset(&sockadr,0,sizeof(sockadr));
	sockadr.sin_family = AF_INET;
	sockadr.sin_port = htons(1234);
	sockadr.sin_addr.s_addr =inet_addr("127.0.0.1");
	int bind_res = bind(sf,(struct sockaddr *)&sockadr,sizeof sockadr);
	if(bind_res < 0)
	errorrep("bind()");

	theiradr.sin_family = AF_INET;
	theiradr.sin_port = htons(1111);
	theiradr.sin_addr.s_addr=inet_addr("127.0.0.1");
	if ( theiradr.sin_addr.s_addr == INADDR_NONE )
	errorrep("bad address.");

	char *username;
	username = malloc(10*sizeof(char));
	char *passwd;
	passwd = malloc(10*sizeof(char));
	int te = sizeof theiradr;	
	int r_res = recvfrom(sf,username,10,0,(struct sockaddr *)&theiradr,&te);
	if(r_res < 0)
	errorrep("recvfrom()");
	r_res = recvfrom(sf,passwd,10,0,(struct sockaddr *)&theiradr,&te);
	if(r_res < 0)
	errorrep("recvfrom()");
//	printf("%s",username);
//	printf("%s",passwd);
	FILE *fp;
	fp = fopen("login","r+");
	char *ubuffer;
	char *pbuffer;
	ubuffer = malloc(10*sizeof(char));
	pbuffer = malloc(10*sizeof(char));
	while(1)
	{//	printf("here\n");
	//	ubuffer = malloc(10*sizeof(char));
	  //      pbuffer = malloc(10*sizeof(char));
		int p = fscanf(fp,"%s",ubuffer);
		p = fscanf(fp,"%s",pbuffer);
//		printf("%d\n",p);
//		printf("%s\n",ubuffer);
//		printf("%s\n",username);
//		printf("%s\n",passwd);
		int cmp = strcmp(username,ubuffer);
		int cmp1 = strcmp(passwd,pbuffer);
//		printf("%d %d\n",cmp,cmp1);
		if(p < 0)
		{
			char *msg = "username not found";
			sendto(sf,msg,20,0,(struct sockaddr *)&theiradr,sizeof(theiradr));
			break;
		}
		if(cmp == 0)
		{
			if(cmp1 == 0)
			{
				char *msg = "login sucess";
				sendto(sf,msg,20,0,(struct sockaddr *)&theiradr,sizeof(theiradr));
			}
			else	
			{	
				char *msg = "authentication failure";
                                sendto(sf,msg,20,0,(struct sockaddr *)&theiradr,sizeof(theiradr));
			}
		}
		
	}
	char command;
	r_res = recvfrom(sf,&command,1,0,(struct sockaddr *)&theiradr,&te);
	if(strcmp(&command,"1"))
	{
		printf("receiving file from client\n");
		rfile(sf);
	}


//	system("netstat -tapen | grep 1234");
//	int lis_status = listen(sf,3);
//	if(lis_status < 0)
//	errorrep("listen()");
//	printf("litstening of port 1234\n");
//	system("netstat -tapen | grep 1234");
//	int sin_size = sizeof(struct sockaddr_in);
//	int accept_fd = accept(sf,&theiradr,&sin_size);
//	if(accept_fd < 0)
/*	errorrep("accept()");
	printf("connected\n");
	int total = 0;
	FILE *nf;
	nf = fopen("temp","w+");
	if(nf == NULL)
	printf("unable to open file");
	int rcv_bytes;
	while(1)
	{
//		printf("entered while\n");
		int rcv_bytes = recv(accept_fd,buffer,256,0);
		if(rcv_bytes == 0 )
		{
			printf("entering break");
			break;
		}
		total = total + rcv_bytes;
		printf("receiving %d\n",rcv_bytes);
		fprintf(nf,"%s",buffer);
//		bzero(buffer,256);
	}
	printf("received %d bytes\n",total);
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
//	close(accept_fd);
}
