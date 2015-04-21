#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>

void errorrep(const char *reason)
{
        perror(reason);
        exit(1);
}

void main()
{
        int cfd,con_fd;
        FILE *fd;
        cfd = socket(PF_INET,SOCK_STREAM,0);
        if(cfd < 0)
                errorrep("socket");
        struct sockaddr_in serveraddr;
        memset(&serveraddr,0,sizeof(serveraddr));
        serveraddr.sin_family=AF_INET;
        serveraddr.sin_port=htons(1234);
        serveraddr.sin_addr.s_addr=inet_addr("127.0.0.1");
        con_fd = connect(cfd,&serveraddr,sizeof(serveraddr));
        if(con_fd < 0)
                errorrep("connect()");
        printf("enter file name: ");
        char *filename;
        filename = malloc(10*sizeof(char));
        scanf("%s",filename);
        fd = fopen(filename,"r+");
        if(fd == NULL)
		 errorrep("file not found");
	cfd = fd;
	while(1)
	{
		int temp = 0;
	}
