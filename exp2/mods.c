#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<string.h>
#include<netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>


void errorrep(const char *reason)
{
	perror(reason);
	exit(1);
}

int creatsocket()
{
	printf("creating a socket...\n");
	int i = socket(PF_INET,SOCK_DGRAM,0);
	if(i < 0)
		errorrep("failed to create socket");
	return i;
}
void dobind(int socket,struct sockaddr_in addr)
{
	int res = bind(socket,(struct sockaddr *)&addr,sizeof(addr));
	if(res < 0)
		errorrep("failed to bind");
}

void main()
{
	//decaling variables for the sockets

	struct sockaddr_in myadr, clientadr;

	//details of the server address
	myadr.sin_family = AF_INET;
	myadr.sin_port = htons(1234);
	myadr.sin_addr.s_addr = inet_addr("127.0.0.1");
	if(myadr.sin_addr.s_addr == INADDR_NONE)
		errorrep("Bad Address Server");

	//details of client address
	clientadr.sin_family = AF_INET;
	clientadr.sin_port = htons(1111);
	clientadr.sin_addr.s_addr = inet_addr("127.0.0.1");
	if(clientadr.sin_addr.s_addr == INADDR_NONE)
		errorrep("Bad Address Client");

	//creating a new socket
	int ssocket = creatsocket();
	
	//binding the socket to serveraddr
	dobind(ssocket,myadr);
	
	char cbit;
	int len = sizeof(clientadr);
	FILE *fp;
	fp = fopen("DNS","r+");
	if(fp == NULL)
	perror("file open failed");
	while(1)
	{
		fseek(fp,0,SEEK_SET);
		char hostname[30];
		recvfrom(ssocket,&cbit,1,0,(struct sockaddr *)&clientadr,&len);
		recvfrom(ssocket,&hostname[0],30,0,(struct sockaddr *)&clientadr,&len);

		char hostbuf[30];
		char ipbuf[30];
		int read = fscanf(fp,"%s",&hostbuf[0]);
//		printf("here\n");
		printf("look up for %s......\n",&hostname[0]);
		int ird = fscanf(fp,"%s",&ipbuf[0]);
		while(read >= 0)
		{
		//	printf("comparing %s",&hostname[0]);
		//	printf(" %s\n",&hostbuf[0]);
			if(strcmp(&hostname[0],&hostbuf[0]) == 0)
			{
				sendto(ssocket,&ipbuf[0],30,0,(struct sockaddr *)&clientadr,len);
				break;
			}
			read = fscanf(fp,"%s",&hostbuf[0]);
        	        ird = fscanf(fp,"%s",&ipbuf[0]);

		}
		if(read < 0)
		{
			printf("here\n");
			struct hostent *hp;
			hp = gethostbyname(&hostname[0]);
			if(hp == NULL)
			{	char msg[30] = "no domain found";		
				sendto(ssocket,&msg[0],30,0,(struct sockaddr *)&clientadr,len);
			}
			else
			{
				fseek(fp,0,SEEK_END);
				fprintf(fp,"%s",&hostname[0]);
				fprintf(fp," ");
//				fprintf(fp,"%s",inet_ntoa( *(struct in_addr *)hp->h_addr_list[0]));
				char *ip;
				ip = malloc(30*sizeof(char));
				ip = inet_ntoa( *(struct in_addr *)hp->h_addr_list[0]);
				fprintf(fp,"%s",ip);
				sendto(ssocket,&ip[0],30,0,(struct sockaddr *)&clientadr,len);
				fprintf(fp,"\n");
			}
		}
	}
	fclose(fp);
	close(ssocket);
}
