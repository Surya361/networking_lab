#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<pthread.h>


typedef struct
{
	int sock;
	struct sockaddr address;
	int addr_len;
} connection_t;
typedef struct
{
	int sock;
	struct sockaddr_in address;
	int addr_len;
} adrs;
typedef struct
{
	int sno;
	int ack;
	char payload[10];
} rdt_packet;

void * connection(void * pt)
{
	FILE *k;
	k = fopen("peer1","r+");
	if(k!= NULL)
	printf("opened peer1 file 1\n");
	adrs * conne;
	conne = (adrs *)pt;
	int times = 0;
	int res = connect(conne->sock,(struct sockaddr *)&conne->address,conne->addr_len);
	times++;
	printf("trying connection try: %d\n",times);
	while(res < 0)
	{
		res = connect(conne->sock,(struct sockaddr *)&conne->address,conne->addr_len);
		printf("trying connection try %d",times);
		times++;
	}
	printf("connected");
	char *buffer;
	buffer = malloc(128*sizeof(char));
	int red = fscanf(k,"%s",buffer);
	while(red > 0)
	{
		printf("sending: %s\n",buffer);
		int len = strlen(buffer);
		send(conne->sock,buffer,len,0);
		free(buffer);
		buffer = malloc(128*sizeof(char));
		red = fscanf(k,"%s",buffer);
	}
	fclose(k);
	close(conne->sock);
	free(conne);
	pthread_exit(0);
	
	
}
void * process(void * ptr)
{
	FILE *p;
	p = fopen("rcv1","r+");
	fseek(p,0,SEEK_END);
	printf("connection here\n");
	char *buffer;
	buffer = malloc(128*sizeof(char));
	connection_t * conn;
	long addr = 0;
	if(!ptr)
	pthread_exit(0);
	conn = (connection_t *)ptr;
	int len = recv(conn->sock,buffer,128,0);
	while(len)
	{
		fprintf(p,"%s",buffer);
		free(buffer);
		buffer = malloc(128*sizeof(char));
		len = recv(conn->sock,buffer,128,0);
	}
	fclose(p);
	close(conn->sock);
	free(conn);
	printf("exiting thread\n");
	pthread_exit(0);
	
}

void errorrep(const char *reason)
{
	perror(reason);
	exit(1);
}

int gettcpsock()
{
	int a = socket(PF_INET,SOCK_STREAM,0);
	if(a < 0)
	perror("tcpsocket()");
	else
	return a;
}
void bindsock(int sock,struct sockaddr_in address)
{
	int res = bind(sock,(struct sockaddr *)&address,sizeof address );
	if(res < 0)
	errorrep("bind err");
}

void startlisten(int sock,int connections)
{
	int res = listen(sock,connections);
	if(res < 0)
	errorrep("listen error");
}

int getport(char *go)
{
	int i=0;
	char *word;
	word = malloc(4*sizeof(char));
	while(*(go + i) != ' ')
	{
		*(word+i) = *(go + i);
		i++;
	}
	*(word+i) = '\0';
	int port = atoi(word);
	return port;
}	

char * getdata(int socket,int length)
{
	char *buffer;
	buffer = malloc(length*sizeof(char));
	int bytes = recv(socket,buffer,length,0);
	if(bytes < 0)
	errorrep("recv failed");
	return buffer;
}

void main(int argc,char *argv[])
{
	int socket,client;
	struct sockaddr_in address;
	int port=1234;

	//connection_t  connection;
	pthread_t tid1,tid2;
	
	//server address
	struct sockaddr_in serveraddr;
	memset(&serveraddr,0,sizeof(serveraddr));
	serveraddr.sin_addr.s_addr = INADDR_ANY;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);

	socket = gettcpsock();
	client = gettcpsock();

	//binding
	bindsock(socket,serveraddr);
	
	//listening
	startlisten(socket,10);

	adrs *conec;
	conec = malloc(sizeof(adrs));
	conec->sock = p2;
//	struct sockaddr pee2 = (struct sockaddr *) peer2;
	conec->address = peer2;
	conec->addr_len = sizeof(peer2);
	pthread_create(&tp2,0,connection,(void *)conec);
	pthread_detach(tp2);

	adrs *co;
	co = malloc(sizeof(adrs));
	co->sock = p3;
	co->address = peer3;
	co->addr_len = sizeof(peer3);
	pthread_create(&tp3,0,connection,(void *)co);
	pthread_detach(tp3);

	adrs *con;
	con = malloc(sizeof(adrs));
	con->sock = p4;
	con->address = peer4;
	con->addr_len = sizeof(peer4);
	pthread_create(&tp4,0,connection,(void *)con);
	pthread_detach(tp4);
	connection_t *connection;
	while(1)
	{
		connection = malloc(sizeof(connection_t));
		connection->sock = accept(socket, &connection->address, &connection->addr_len);
		if (connection->sock <= 0)
		{
			free(connection);
			errorrep("accept");
		}
		else
		{
			/* start a new thread but do not wait for it */
			pthread_create(&thread, 0, process, (void *)connection);
			pthread_detach(thread);
		}
		
	}	
}