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
	if(argc != 2)
	errorrep("invalid number of arguments\n");
	
	//getting port number
	int port = atoi(argv[1]);
//	printf("%d",port);
	
	//declaring sockets
	int mysocket,consocket;
	
	//getting sockets
	mysocket = gettcpsock();

	//declaring server address
	struct sockaddr_in serveraddr;
	memset(&serveraddr,0,sizeof(serveraddr));
	serveraddr.sin_addr.s_addr = INADDR_ANY;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);

	//binding the server to the address
	bindsock(mysocket,serveraddr);
	
	//litsening to the port
	startlisten(mysocket,4);
	
	//accpting a connection
	struct sockaddr_in conaddress;
	int len = sizeof(conaddress);
	consocket = accept(mysocket,(struct sockaddr *)&conaddress,&len);
	if(consocket < 0)
	errorrep("connection failed");
	
	//get some data
	char *data;
	data = getdata(consocket,1000);
	printf("%s",data);
	
	//getting the method and the hostname
	char *method, *host;
	method = malloc(10*sizeof(char));
	host = malloc(30*sizeof(char));
	int i=0;
	while(*(data + i) != ' ')
	{
		*(method + i) = *(data + i);
		i++;
	}
	*(method + i) = '\0';
	int j = 0;
	i++;
	i = i+7;

	//getting the host name and port
	int por=80;
	while(*(data + i + j) != ' ')
	{
		if(*(data + i + j) == ':')
		{
			por = getport((data+i+j+1));
			break;
		}
		*(host + j) = *(data + i + j);
		j++;
	}
	if(*(host + j - 1) == '/')
	*(host + j -1) = '\0';
	*(host + j) = '\0';
	

	printf("\n%s\n",method);
	printf("%s\n",host);
	printf("%d\n",por);

	//DNS lookup
	struct hostent *hp;
	hp = gethostbyname(host);
	if(hp == NULL)
	errorrep("DNS lookupfailed");
	
	//trying different ipaddress
	int num = 0;
	int cfd,cres;
	struct sockaddr_in hosts;
	for(num = 0; hp->h_addr_list[num]; ++num)
	{
		memset(&hosts,0,sizeof(hosts));
		hosts.sin_family = AF_INET;
		hosts.sin_port = htons(por);
		hosts.sin_addr = *(struct in_addr *)hp->h_addr_list[num];
		cfd = socket(PF_INET,SOCK_STREAM,0);
		cres = connect(cfd,(struct sockaddr *)&hosts,sizeof(hosts));
		printf("Address:\t%s\n",inet_ntoa( *(struct in_addr *)hp->h_addr_list[num]));
		if(cres > 0)
		break;
	}
	if(cres < 0)
	errorrep("host down");
//	char ret = '\r';
//	char line = '\n';
	printf("connection established\n");
//	char buffer[300];
//	strcpy(&buffer[0],"GET http://www.nitc.ac.in HTTP/1.1\r\nHost: localhost 80\r\n\r\n");
	int bytes_send = send(cfd,data,1000,0);
	if(bytes_send < 0)
	errorrep("data sending failed");
//	else
//	write(cfd,&ret,sizeof(ret));
//	write(cfd,&line,sizeof(ret));
//	int z = shutdown(cfd,SHUT_WR);
//	if(z<0)
//	errorrep("shutdown failed");
//	printf("%s",data);	
//	printf("%d bytes of data sent of %d",bytes_send,strlen(data));
	char *returndata;
	returndata = malloc(1000*sizeof(char));
	int reci = recv(cfd,returndata,sizeof(returndata),0);
	if(reci <0 )
		printf("failed");
	while(reci > 0 )
	{
//		prin("printing data\n");
		printf("%s",returndata);
		send(consocket,returndata,strlen(returndata),0);
		reci = recv(cfd,returndata,sizeof(returndata),0);
	}
//	char *returndata;
//	returndata = getdata(cfd,100000);
	
//	printf("%s",returndata);
	close(cfd);
	close(mysocket);
	close(consocket);
}
