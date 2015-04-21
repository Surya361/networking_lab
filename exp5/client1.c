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
void print(struct pack *ptr)
{
	int i =0;
	while( i < 100)
	{
		if(((ptr+i)->sno) == -1)
		{
			i++;
			continue;
		}
		printf("packetno. %d: %s\n",((ptr+i)->sno),(ptr+i)->payload);
		i++;
	}
}
int sendpacket(struct pack *buffer,int *window,int *base,int *nextseq,int socket)
{
	printf("sending mainstream packets\n");
	int next = *nextseq;
	int bas = *base;
	int i;
	printf("base: %d nextseq:%d\n",bas,next);
	for(i=bas;i<next;i++)
	{
		if(bas == -1)
		{
			printf("sending packet %d: %s\n",next,(buffer+next)->payload);
			send(socket,(buffer+next),sizeof(struct pack),0);
			bas++;
			*base = bas;
			*nextseq = 1;
			continue;
		}
		if(((buffer + i)->ackno) == -1)
			send(socket,(buffer+next),sizeof(struct pack),0);
	}
	next = *nextseq;
	while((( (*nextseq) - (*base) ) < (*window) ) && ( (buffer+ next)->sno != -1))
	{
		printf("sending packet sno %d: %s\n",next,(buffer+next)->payload);
		send(socket,(buffer+next),sizeof(struct pack),0);
		next++;
		*nextseq = next;
	}
	if((buffer+next)->sno == -1 )
		exit(0);

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
	printf("acknoledgement received : %d\n",acknno);
	//exit(1);
	(buffer+acknno)->ackno = 1;
	int i;
	for(i = *base;i<100;i++)
	{
		if((buffer+i)->ackno == 1)
			*base = *base + 1;
		else
			break;
	}
}

int dataprocess(struct pack *packet,struct pack *buffer)
{
	int seqno = packet->sno;
	if((buffer + seqno)->ackno == 1)
	{
		//printf("duplicate packet seqno %d received\n",seqno);
		printf("received duplicate packetno %d: %s\n",seqno,(buffer+seqno)->payload);
		return 1;
	}
	(buffer + seqno)->ackno = 1;
	strcpy((buffer+seqno)->payload , packet->payload);
	printf("received packetno %d: %s\n",seqno,(buffer+seqno)->payload);
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
		strcpy((buffer+i)->payload , a);
		//printf("a: %s\n",a);
		//printf("buffer: %s\n",(buffer+i)->payload);
		i++;
	}
	
}

void main(int argc,char *argv[])
{
	int port = 1235;
	fd_set read_fds,master_fds;
	int fdmax;
	int ssocket;
	struct sockaddr_in remoteaddr;

	//zeroing fds
	FD_ZERO(&read_fds);
	FD_ZERO(&master_fds);

	//getting socket
	ssocket = gettcpsock();
	fdmax = ssocket;
	//declaring server address
	struct sockaddr_in serveraddr;
	memset(&serveraddr,0,sizeof(serveraddr));
	serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);

	
	//adding listener 
	FD_SET(ssocket,&master_fds);

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

	//make packets invalid
	makeinvalid(datato);
	makeinvalid(datafrom);

	//makepackets
	makepackets(datato,p);
	//print(datato);

	int sendbase = -1;
	int nextseq = 0;
	//litsening to port
//	startlisten(ssocket,10);
	int res;
	//connecting to the server
	int cstatus = -1;
	while(cstatus < 0)
	{
		printf("here\n");
		cstatus = connect(ssocket,(struct sockaddr *)&serveraddr,sizeof(serveraddr));
	}	
	int i;
	int sendsocket=-1;

	struct timeval tout;
	tout.tv_sec = 1;
	tout.tv_usec = 0;
	int z;
	while(1)
	{
		read_fds = master_fds;
		//printf("was here\n");
		for(z=0;z<window*3;z++)
		{	
		if (select(fdmax+1, &read_fds, NULL, NULL, &tout) == -1) 
			errorrep("select");

		//printf("after select\n");

			for(i=0; i <= fdmax; i++)
			{
				//printf("checking socket %d\n",i);
				if(FD_ISSET(i,&read_fds))	
				{
					
					
					
						//printf("incomming packet\n");
						struct pack incomming;
						res = recv(i,&incomming,sizeof(incomming),0);
						if(res == 0)
						{
							printf("connection closed by socket: %d\n",i);
							FD_CLR(i,&master_fds);
						}
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
				
				sendpacket(datato,&window,&sendbase,&nextseq,ssocket);
				int ti = time(NULL);
				while((time(NULL)) < ti+rtt)
				{
					int a=0;
					a++; //does nothing
				}	
			
			
	}
	
	
}

