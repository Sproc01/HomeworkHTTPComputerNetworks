#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>

int main()
{
	int s, t;
	unsigned char* p;
	char* request="GET / \r\n";
	char response[10001];	

	if(-1==(s=socket(AF_INET,SOCK_STREAM,0)))
	{
		perror("Socket fail");
		printf("%d\n", errno);
		return 1;
	}
	struct sockaddr_in remote_addr;
	remote_addr.sin_family=AF_INET;
	remote_addr.sin_port=htons(80);
	p=(unsigned char*)&remote_addr.sin_addr.s_addr;
	p[0]=142;
	p[1]=250;
	p[2]=200;
	p[3]=36;
	t=connect(s,(struct sockaddr*)&remote_addr, sizeof(struct sockaddr_in));
	if(t==-1)
	{
		perror("Connect fail");
		printf("%d\n",errno);
		return 1;
	}
	t=write(s, request,1000);
	sleep(1);
	while(t=read(s,response,10000))
	{
		response[t]=0;
		printf("%s\n", response);
		printf("%d\n", t);
	}
	return 0;
}
