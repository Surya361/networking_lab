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
	int sno;
	int ackno;
	char payload[20];
};

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

int sendpacket(struct pack *buffer,int *window,int *base,int *nextseq,int socket)
{
	int next = *nextseq;
	int bas = *base;
	int i;
	
	for(i=bas;i<next;i++)
	{
		if(bas == -1)
		{
			send(socket,(buffer+next),sizeof(struct pack),0);
			bas++;
			*base = bas;
			*nextseq = *nextseq + 1;
			continue;
		}
		if(((buffer + i)->ackno) == -1)
			send(socket,(buffer+next),sizeof(struct pack),0);
	}
	next = *nextseq;
	while((( (*nextseq) - (*base) ) < (*window) ) && ( (buffer+ next)->sno != -1))
	{
		printf("sending packet sno %d",next);
		send(socket,(buffer+next),sizeof(struct pack),0);
		next++;
		*nextseq = next;
	}

}

void makeinvalid(struct pack *ptr)
{
	int i;
	for(i=0;i<100;i++)
	{
		(ptr+i)->sno = -1;
		(ptr+i)->ackno = -1;
	}
}
void ackprocess(struct pack *packet,struct pack *buffer,int *base)
{
	int acknno = packet->ackno;
	(buffer+acknno)->ackno == 1;
	int i;
	for(i = *base;i<100;i++)
	{
		if((buffer+i)->ackno == 1)
			*base++;
		else
			break;
	}
}

int dataprocess(struct pack *packet,struct pack *buffer)
{
	int seqno = packet->sno;
	if((buffer + seqno)->ackno == 1)
	{
		printf("duplicate packet seqno %d received\n",seqno);
		return 1;
	}
	(buffer + seqno)->ackno = 1;
	strcpy((buffer+seqno)->payload , packet->payload);
	return 1;

}
void makepackets(struct pack *buffer,FILE *fp)
{
	int i=0,read = 1;
	while(read != 0)
	{
		char a[20];
		read = fread(a,sizeof(char),19,fp);
		a[read] = '\0';
		(buffer + i)->sno = i;
		(buffer + i)->ackno = -1;
		strcmp((buffer+i)->payload , a);
		i++;
	}
	
}

void main(int argc,char *argv[])
{
	int port = 1234;
	fd_set read_fds;
	int fdmax;
	int ssocket;
	struct sockaddr_in remoteaddr;

	//zeroing fds
	FD_ZERO(&read_fds);

	//getting socket
	ssocket = gettcpsock();

	//declaring server address
	struct sockaddr_in serveraddr;
	memset(&serveraddr,0,sizeof(serveraddr));
	serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);

	//binding the peer to address
	bindsock(ssocket,serveraddr);
	
	//adding listener 
	FD_SET(ssocket,&read_fds);

	//setting rtt and window size
	int window,rtt;
	printf("enter window size: ");
	scanf("%d",&window);
	printf("enter rtt: ");
	scanf("%d",&rtt);

	char str[30];
	printf("enter the file to be sent: ");
	scanf("%s",str);

	//open the file
	FILE *p;
	p = fopen(str,"r+");

	//packets and ackbuffer
	struct pack *datato;
	datato = malloc(100*sizeof(struct pack));
	struct pack *datafrom;
	datafrom = malloc(100*sizeof(struct pack));

	//makepackets
	makepackets(datato,p);

	makeinvalid(datato);
	makeinvalid(datafrom);


	int sendbase = -1;
	int nextseq = 0;
	//litsening to port
	startlisten(ssocket,10);
	int res;

	int i;
	int sendsocket;
	
	while(1)
	{
		if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) 
		{
	        perror("select");
	        exit(4);
	    }
			for(i=0; i<=fdmax; i++)
			{
				if(FD_ISSET(i,&read_fds))	
				{
					if(i == ssocket)
					{
						struct sockaddr_in remote;
						int addrlen = sizeof(remote);
						int newfd = accept(ssocket,(struct sockaddr *)&remote,&addrlen);
						sendsocket = newfd;
						if(newfd < 0)
							errorrep("accept");
						else
						{
							FD_SET(newfd,&read_fds);
							if(newfd > fdmax)
							fdmax = newfd;
							printf("selectserver: new connection on socket %d\n",newfd);
						}
							
					}
					else
					{
						struct pack incomming;
						res = recv(i,&incomming,sizeof(incomming),0);
						if(res == 0)
							printf("connection closed by socket: %d\n",i);
						if(res < 0)
							errorrep("reciving packet failed:");
						if(incomming.sno == -1)
							ackprocess(&incomming,datato,&sendbase);
						if(incomming.ackno == -1)
						{
							dataprocess(&incomming,datafrom);
							struct pack ackpacket;
							ackpacket.sno = -1;
							ackpacket.ackno = incomming.sno;
							send(i,&ackpacket,sizeof(ackpacket),0);
						}	
								

					}
				}
			}
			sendpacket(datato,&window,&sendbase,&nextseq,sendsocket);
			int ti = time(NULL);
			while((time(NULL)) > ti+rtt)
			{
				int a=0;
				a++; //does nothing
			}

	}
	
	
}

