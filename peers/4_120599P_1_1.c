#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netdb.h>

struct pack
{
	int type;
	int prt;
	char filenames[100];
};
struct packlist
{
	int pr;
	char files[100];
};

void printlist(struct packlist *h)
{
	int i = 0;
	while((h+i)->pr != 0 )
	{
		printf("%d %s\n",(h+i)->pr,(h+i)->files);
		i++;
	}
}

void errorrep(const char *reason)
{
	perror(reason);
	exit(1);
}

int gettcpsock()
{
	printf("creating socket..........\n");
	int a = socket(PF_INET,SOCK_STREAM,0);
	if(a < 0)
	perror("socket()");
	else
	printf("socket creation sucessful...........\n");
	return a;
}

void bindsock(int sock,struct sockaddr_in address)
{
	printf("binding socket............\n");
	int res = bind(sock,(struct sockaddr *)&address,sizeof(address));
	if(res < 0)
	errorrep("bind err");
	else
	printf("binding socket sucessfull..........\n");
}

void startlisten(int sock,int connections)
{
	printf("starting to listen..........\n");
	int res = listen(sock,connections);
	if(res < 0)
	errorrep("listen error");
	else
	printf("listening..............\n");
}

int sendall(int s, char *buf, int *len)
{
        int total = 0;
        int bytesleft = *len;
        int n;

        while(total < *len)
        {
                n = send(s, buf+total, bytesleft, 0);
                if (n == -1) { break; }
                total += n;
                bytesleft -= n;
        }

        *len = total;
        return n==-1?-1:0;
}

void main(int argc,char *argv[])
{
	/*if(argc != 2)
	errorrep("invalid number of arguments\n");
	
	//getting port number
	int port = atoi(argv[1]);*/

	fd_set master;
	fd_set read_fds;
	fd_set write_fds;
	int fdmax;
	int lispeer,p2,p3,p4;
	int newpeer;
	struct sockaddr_in remoteaddr;

	//zeroing fds
	FD_ZERO(&master);
	FD_ZERO(&read_fds);
	FD_ZERO(&write_fds);

	//getting socket
	lispeer = gettcpsock();
	p2 = gettcpsock();
	p3 = gettcpsock();
	p4 = gettcpsock();
	

	//declaring peer2 address
	struct sockaddr_in serveraddr;
	memset(&serveraddr,0,sizeof(serveraddr));
	serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(3301);
	
	//declaring peer3 address
	struct sockaddr_in paddr;
	memset(&paddr,0,sizeof(serveraddr));
	paddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	paddr.sin_family = AF_INET;
	paddr.sin_port = htons(3302);
	
	//decalring peer4 address
	struct sockaddr_in peaddr;
	memset(&peaddr,0,sizeof(serveraddr));
	peaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	peaddr.sin_family = AF_INET;
	peaddr.sin_port = htons(3303);
	
	//declaring server address
	struct sockaddr_in peeraddr;
	memset(&peeraddr,0,sizeof(peeraddr));
	peeraddr.sin_addr.s_addr = INADDR_ANY;
	peeraddr.sin_family = AF_INET;
	peeraddr.sin_port = htons(3300);

	//binding the peer to address
	bindsock(lispeer,peeraddr);
	
	//adding listener 
	FD_SET(lispeer,&master);
	FD_SET(lispeer,&read_fds);
	FD_SET(p2,&write_fds);
	FD_SET(p3,&write_fds);	
	FD_SET(p4,&write_fds);


	//litsening to port
	startlisten(lispeer,10);
	
	fdmax = lispeer;
	if(fdmax < p2)
		fdmax = p2;
	if(fdmax < p3)
		fdmax = p3;
	if(fdmax < p4)
		fdmax = p4;
	
/*	//register with the dir server
	char filename[10];
	printf("enter names of files with the peer with spaces:");
	scanf("%[^\n]s",&filename[0]);
	fdmax = lispeer;
	int dserv = gettcpsock();
	if(fdmax < dserv)
	fdmax = dserv;
	int res = connect(dserv,(struct sockaddr *)&serveraddr,sizeof(serveraddr));
	if(res < 0 )
	errorrep("connect");
	struct packlist list[50];
/*	*(buf+0) = '1';
	*(buf+1) = ' ';
	*(buf+2) = '\0';
	strcat(buf,argv[1]);
	strcat(buf,&username[1]);*/
/*	struct pack reg;
	reg.type = 1;
	reg.prt = port;
	strcpy(reg.filenames,&filename[0]);
	int pcklen = sizeof(struct pack);
	send(dserv,&reg,pcklen,0);
	struct sockaddr_in filepeer,remote;
	FILE *put;*/
//	connect(p2,(struct sockaddr *)&serveraddr,sizeof(serveraddr));
//	connect(p3,(struct sockaddr *)&paddr,sizeof(paddr));
//	connect(p4,(struct sockaddr *)&peaddr,sizeof(peaddr));
	int i;
	FILE *fp;
	fp = fopen("peer1","r+");
	char *buffer;
	int connections = 0;
	int newfd;
	for(;;)
	{
		FILE *p;
		p = fopen("rcv1","r+");
		if (select(fdmax+1, &read_fds,NULL, NULL,NULL) == -1) 
		{
         	   perror("select");
          	   exit(4);
        	}
		for(i=0;i<=fdmax;i++)
		{
			if(FD_ISSET(i,&read_fds))
			{
				if(i == lispeer)
				{
					int addrlen = sizeof remoteaddr;
					newfd = accept(lispeer,(struct sockaddr *)&remoteaddr,&addrlen);
					if(newfd < 0)
					errorrep("accept");
					else
					{
						printf("some one knocking connections\n");
						FD_SET(newfd,&read_fds);
						if(newfd > fdmax)
						fdmax = newfd;
						printf("selectserver: new connection on socket %d\n",newfd);
						if(connections == 0)
							connect(p2,(struct sockaddr *)&serveraddr,sizeof(serveraddr));
						else
							if(connections == 1)
							connect(p3,(struct sockaddr *)&paddr,sizeof(paddr));
							else
								if(connections == 2)
									connect(p4,(struct sockaddr *)&peaddr,sizeof(peaddr));
						connections++;
						printf("connection number %d\n",connections);
					}
					
				}
				else
				{
					printf("in else\n");
					int nbytes;
					buffer = malloc(4*sizeof(char));
					if((nbytes = recv(i,buffer,4*sizeof(char),0)) <= 0)
						{
							if(nbytes == 0)
							printf("connection closed by socket %d\n",i);
							else
							errorrep("recv");
							FD_CLR(i,&read_fds);
							close(i);
						}
					else
						{
							printf("recived %d bytes\n",nbytes);
							//fread(buffer,sizeof(char),4,p);
							fseek(p,0,SEEK_END);
							int j;
							for(j=0;j<4;j++)
							fprintf(p,"%c",*(buffer+j));
							free(buffer);
						}
					
					int k;
					if(connections == 3)
					{
						buffer = malloc(4*sizeof(char));
						fread(buffer,sizeof(char),4,fp);
						/*for(k=0;k<=fdmax;k++)
						{
							if(FD_ISSET(k,&write_fds))
							{
								int sbytes = send(k,buffer,4*sizeof(char),0);
								printf("sent %d to %d",sbytes,k);
							}
					
						}*/
						send(p2,buffer,4,0);
						send(p3,buffer,4,0);
						send(p4,buffer,4,0);
					}
				}
			}
				
			
		}
		fclose(p);
	}
	/*while(1)
	{
		char name[10];
		printf("enter file to be downloaded:");
		scanf("%s",&name[0]);

		struct pack req;
		req.type=2;
		req.prt = 0;
		strcpy(req.filenames,name);
		res = send(dserv,&req,sizeof(struct pack),0);
		if(res < 0)
		perror("req message failed to be sent");

		int fileport;
		res = recv(dserv,&fileport,sizeof(fileport),0);
		if(res < 0)
		perror("server failed to send the list");
		printf("received %d as the fileport\n",fileport);
		printf("do you want to get the file 1-yes 0-no\n");
		int answer;
		scanf("%d",&answer);
		if(answer)
		{
			int getfile = gettcpsock();
			memset(&filepeer,0,sizeof(filepeer));
			filepeer.sin_addr.s_addr = inet_addr("127.0.0.1");
			filepeer.sin_family = AF_INET;
			filepeer.sin_port = htons(fileport);

			res = connect(getfile,(struct sockaddr *)&filepeer,sizeof(filepeer));
			if(res < 0)
			errorrep("connection to getfile failed");

			if(fdmax < getfile)
			fdmax = getfile;
			FD_SET(getfile,&read_fds);


			res = send(getfile,&req,sizeof(struct pack),0);
			if(res < 0)
			errorrep("send file req");


			if (select(fdmax+1, &read_fds, &write_fds, NULL, NULL) == -1) 
			{
	         	   perror("select");
	          	   exit(4);
	        	}
			FILE *get;
			get = fopen(&name[0],"w+");
			int i;
			for(;;)
			{
			for(i=0; i<=fdmax; i++)
			{
				if(FD_ISSET(i,&read_fds))	
				{
					if(i == lispeer)
					{
						int addrlen = sizeof remote;
						int newfd = accept(lispeer,(struct sockaddr *)&remote,&addrlen);
						if(newfd < 0)
						errorrep("accept");
						else
						{
							FD_SET(newfd,&write_fds);
							if(newfd > fdmax)
							fdmax = newfd;
							printf("selectserver: new connection on socket %d\n",newfd);
							struct pack filereq;
							int res = recv(newfd,&filereq,sizeof(filereq),0);
							if(res < 0)
							errorrep("here\n");
							put = fopen(filereq.filenames,"r+");
						}
						
					}
					else
					{
						char buf[256];
						res = recv(i,buf,sizeof(buf),0);
						if(res == 0)
						printf("connection closed by socket %d\n",i);
						if(res < 0)
						{
							errorrep("recv");
							FD_CLR(i,&master);
							close(i);
						}
						printf("getting: %s\n",buf);
						fprintf(get,"%s",buf);
						//free(buf);
					}
				if(FD_ISSET(i,&write_fds))
					{
						char buf[256];
						int nu = fscanf(put,"%[^\n]s",&buf[0]);
						if(nu < 0)
						shutdown(i,2);
						else
						printf("sending: %s\n",buf);
						res = send(i,buf,sizeof(buf),0);
						//free(buf);
					}
				}
			}
			
			}			
			
		}
	}*/
	
}

