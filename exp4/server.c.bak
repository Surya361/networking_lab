#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netdb.h>


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

void main()
{
	fd_set master;
	fd_set read_fds;
	int fdmax;
	int listener;
	int newfd;
	char *buf;
	buf = malloc(256*sizeof(char));
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
	
	int addrlen;
	
	FILE *p;
	p = fopen("peer","r+");
	if(p == NULL)
	errorrep("fopen");
	
	for(;;)
	{
		read_fds = master;
		if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) 
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
					if((nbytes = recv(i,buf,sizeof buf,0)) <= 0)
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
							if(buf[0] == '1')
							fprintf(p,"%s",buf+2);
							if(buf[0] == '2')
							{
								printf("request for filename %s",(buf+2));
							}
						}
				}
			}
		}
		
	}
	
	
}
