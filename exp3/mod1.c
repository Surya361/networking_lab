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

int check(char *ur,char *off)
{
	int i=0,j=0,start=0;	
	while(*(ur+start) != '\0')
	{
		if(*(ur+i+start) == *(off+i))
		{
			i++;
			if(*(off+i) == '\0')
			return 1;
		}
		else
		{
			i=0;
			start++;
		}
	}
	return 0;
		
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

	 printf("enter offencive words to check:");
        char offencive[10];
        scanf("%s",&offencive[0]);
	time_t new,old;
	old = time(0);

while(1)
{
	new = time(0);
	if((new-old) > 100)
	{
		printf("timed out");
		exit(6);
	}
	
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

	char *uri;
	uri = malloc(50*sizeof(char));
	sscanf(data,"GET %s HTTP/1.0\r\n\r\n",uri);
	
	int bit = check(uri,offencive);
	if(bit == 1)
	exit(1);	
	
	printf("\n\nhere\n");
	printf("\n%s\n",uri);
	printf("\n\n");
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
	int start = i;
	while(start < i+7)
	{
		*(data + start) = ' ';
		start++;
	}
	i = i+7;

	//getting the host name and port
	int por=80;
	while(*(data + i + j) != ' '&& *(data + i+j) != '/')
	{
		if(*(data + i + j) == ':')
		{
			por = getport((data+i+j+1));
			break;
		}
		*(host + j) = *(data + i + j);
		*(data+i+j) = ' ';
		j++;
	}
	if(*(host + j - 1) == '/')
	*(host + j -1) = '\0';
	*(host + j) = '\0';
	int z = 0;
/*	while(*(data+i+z) != 'H')
	{
		*(data+i+z) = ' ';
		z++;
	}
	*(data+i+2) = '/';*/
	printf("%s",data);

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

	printf("connection established\n");
	int bytes_send = send(cfd,data,1000,0);
	if(bytes_send < 0)
	errorrep("data sending failed");

	char *returndata;
	returndata = malloc(1000*sizeof(char));
	int reci = recv(cfd,returndata,sizeof(returndata),0);
	if(reci <0 )
		printf("failed");
	while(reci > 0)
	{
		
		printf("%s",returndata);
		send(consocket,returndata,strlen(returndata),0);
		reci = recv(cfd,returndata,sizeof(returndata),0);
	}
	free(data);
	free(host);
	free(method);

	close(cfd);
	close(consocket);
}
close(mysocket);
}
