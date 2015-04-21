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
	int port;
	char files[100];
};
void printlist(struct packlist *h)
{
	int i = 0;
	while((h+i)->port != 0 )
	{
		printf("%d %s\n",(h+i)->port,(h+i)->files);
		i++;
	}
}
void errorrep(const char *reason)
{
	perror(reason);
	exit(1);
}
void print(char *a,int len)
{
	int i = 0;
	while(i<len)
	{
		printf("%c",*(a+i));
		i++;
	}
}
int find(char *file,struct packlist *h)
{
	int i = 0;
	while((h+i)->port != 0)
	{
		printf("comparing %s %s",file,(h+i)->files);
		if(strstr((h+i)->files,file) != NULL)
		{
			return (h+i)->port;
		}
		i++;
	}
	return 0;
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

void main()
{
	fd_set master;
	fd_set read_fds;
	int fdmax;
	int listener;
	int newfd;
	struct sockaddr_in remoteaddr;
	
	//zeroing fds
	FD_ZERO(&master);
	FD_ZERO(&read_fds);
	
	//getting socket
	listener = gettcpsock();

	//declaring server address
	struct sockaddr_in serveraddr;
	memset(&serveraddr,0,sizeof(serveraddr));
	serveraddr.sin_addr.s_addr = INADDR_ANY;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(3300);
	
	//binding the server to address
	bindsock(listener,serveraddr);
	
	//adding listener to serv
	FD_SET(listener,&master);
	//litsening to the port
	startlisten(listener,10);
	
	fdmax = listener;
	struct packlist head[50];
	int peercnt = 0;
	
	int addrlen;
	
	//timeout
	struct timeval tv;
	tv.tv_sec = 2;
	tv.tv_usec = 0;	
	for(;;)
	{
		 FILE *p;
	        p = fopen("peer","r+");
        	if(p == NULL)
	        errorrep("fopen");
		
		fseek(p,0,SEEK_END);
		struct pack packet;
		read_fds = master;
		if (select(fdmax+1, &read_fds, NULL, NULL, &tv) == -1) 
		{
         	   perror("select");
          	   exit(4);
        	}
		int i;
		for(i=0; i<=fdmax; i++)
		{
			if(FD_ISSET(i,&read_fds))
			{
				if(i == listener)
				{
					addrlen = sizeof remoteaddr;
					newfd = accept(listener,(struct sockaddr *)&remoteaddr,&addrlen);
					if(newfd < 0)
					errorrep("accept");
					else
					{
						FD_SET(newfd,&master);
						if(newfd > fdmax)
						fdmax = newfd;
						printf("selectserver: new connection on socket %d\n",newfd);
					}
					
				}
				else
				{
					int nbytes;
					if((nbytes = recv(i,&packet,sizeof(struct pack),0)) <= 0)
						{
							if(nbytes == 0)
							printf("connection closed by socket %d\n",i);
							else
							errorrep("recv");
							FD_CLR(i,&master);
							close(i);
						}
					else
						{
							printf("recived %d bytes\n",nbytes);
							if(packet.type == 1)
							{
								fseek(p,0,SEEK_END);
								fprintf(p,"\n");
								int res = fprintf(p,"%d",packet.prt);
								fprintf(p," ");
								fprintf(p,"%s",packet.filenames);
								head[peercnt].port = packet.prt;
								strcpy(head[peercnt].files,packet.filenames);
								printlist(head);
								peercnt++;
							}
							if(packet.type == 2)
							{
								
								printf("socket:%d requesting filename:%s",i,packet.filenames);
								/*int res = send(i,&head[0],sizeof(head),0);
								if(res < 0)
								perror("failed to send dir information\n");
								printf("socket:%d requesting filename:%s",i,packet.filenames);
								fseek(p,0,SEEK_SET);
								char *buffer;
								int rn = fscanf(p,"%[^\n]s",buffer)
								while(r*/
								int number = find(packet.filenames,head);
								printf("found file in %d",number);
								int res = send(i,&number,sizeof(number),0);

							}
						}
				}
			}
		}
//		free(buf);
		fclose(p);
		
	}
	
	
}
