#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>

#define ENTITY_BODY_SIZE 100000

struct headers{
	char* n;
	char* v;
};

struct headers h[100];
int s, t;
unsigned char* p;
char* request="GET / HTTP/1.0\r\n\r\n";
char response[ENTITY_BODY_SIZE+1];
char hbuf[5000];
char sl[1001];

int main()
{
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
	
	int i;
	for(i=0; i<1000 && read(s,sl+i,1) && (sl[i]!='\n' || sl[(i)?i-1:i]!='\r'); i++);
	
	sl[i]=0;
	printf("Status Line---->%s\n",sl);
	int j=0;
	h[j].n=&hbuf[0];
	for(i=0;i<5000 && read(s,hbuf+i,1);i++)
	{
		if(hbuf[i]=='\n' && hbuf[i-1]=='\r')
		{	
			hbuf[i-1]=0;
			if(h[j].n[0]==0) break;
			h[++j].n=hbuf+i+1;
		}
		else if(hbuf[i]==':' && !h[j].v)
		{
			hbuf[i]=0;
			h[j].v=hbuf+i+1;
		}
	}
	
	for(i=0;h[i].n[0];i++)
		printf("h[%d].n --->%s, h[%d].v----->%s\n", i, h[i].n, i, h[i].v);
	
	int n=0;
	for(i=0;i<ENTITY_BODY_SIZE && (n=read(s,response+i,ENTITY_BODY_SIZE-i));i+=n);
	printf("Response:\n%s\n",response);


	return 0;
}
