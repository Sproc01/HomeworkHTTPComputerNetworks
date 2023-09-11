#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

struct header{
	char* n;
	char* v;
};


int s,t;
char* request="GET /http://www.google.com/ HTTP/1.1\r\nHOST:88.80.187.84\r\n\r\n";
struct sockaddr_in server;
char sl[100];
char buffer[2000];
struct header h[500];
char response[10000000];

int main()
{
	s=socket(AF_INET, SOCK_STREAM, 0);
	if(s==-1){perror("Socket error: "); printf("%d\n", errno); return -1;}
	server.sin_family=AF_INET;
	server.sin_port=htons(8082);
	char* ip=(unsigned char*)&server.sin_addr.s_addr;
	ip[0]=88;
	ip[1]=80;
	ip[2]=187;
	ip[3]=84;
	t=connect(s, (const struct sockaddr*)&server, sizeof(struct sockaddr_in));
	if(t==-1){perror("Connect error: "); printf("%d\n",errno); return -1;}
	write(s, request,strlen(request));
	for(t=0;t<100 && read(s, sl+t,1) && !(sl[t]=='\n' && sl[t-1]=='\r');t++);
	sl[t]=0;
	printf("Status line: %s\n", sl);
	int j=0;
	h[j].n=buffer;
	for(t=0; t<1000 && read(s,buffer+t,1); t++)
	{
		if(buffer[t]=='\n' && buffer[t-1]=='\r')
		{
			buffer[t-1]=0;
			if(h[j].n[0]==0) break;
			h[++j].n=buffer+t+1;
		}
		else if(buffer[t]==':' && !h[j].v)
		{
			h[j].v=buffer+t+1;
			buffer[t]=0;
		}
	}
	int L=-1;
	for(int i=0;i<j;i++)
	{
		printf("Header: %s, value: %s\n", h[i].n, h[i].v);
		if(!(strcmp(h[i].n, "Content-Length")))
			L=atoi(h[i].v);
	}
	if(L!=-1)
	{
		t=0;
		printf("Response:\n");
		for(int i=0; i<L && (t=read(s, response, 1000)); i+=t)
			printf("%s", response);
		printf("\n");
	}
	else
	{	
		while(L!=0)
		{
			char chunksize[10];
			int i=0;
			for(i=0;i<10 && read(s, chunksize+i,1) && !(chunksize[i]=='\n' && chunksize[i-1]=='\r');i++);
			chunksize[i]=0;
			L=(int)strtol(chunksize, NULL, 16);
			for(int i=0; i<1000000 && (t=read(s, response+i,L-i));i+=t);
			printf("%s",response);
			printf("\n");
			read(s,chunksize,2);
			if(chunksize[0]!='\r' || chunksize[1]!='\n'){printf("Chunk error"); return -1;}
		}
		printf("End response\n");
	}
}
