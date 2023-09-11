#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>

struct header{
	char* n;
	char* v;
};

int s,t, i,j, s2;
char request[10000];
char response[10000];
struct header h[500];
char hbuffer[1000];
char *method, *filename, *version;
char rq[100];
char command[100];
struct sockaddr_in local;
struct sockaddr_in remote;

int main()
{
	FILE* f;
	int yes=1;
	if((s=socket(AF_INET,SOCK_STREAM, 0))==-1){perror("socket error: "); printf("%d\n", errno); return -1;}	
	setsockopt(s,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int));
	local.sin_family=AF_INET;
	local.sin_port=htons(8081);
	if((t=bind(s, (const struct sockaddr*)&local, sizeof(struct sockaddr_in)))==-1){perror("bind error: "); printf("%d\n", errno); return -1;}
	if((t=listen(s, 10))==-1){perror("listen error: "); printf("%d\n", errno); return -1;}
	
	int len=sizeof(struct sockaddr_in);
	while(1)
	{	
		j=0;
		bzero(hbuffer, 1000);
		bzero(rq, 100);
		bzero(request,10000);
		bzero(response, 1000);
		if((s2=accept(s,(struct sockaddr*)&remote, &len))==-1){perror("accept error: "); printf("%d\n", errno); return -1;}
		for(i=0; i<100 && read(s2, rq+i,1) && !(rq[i]=='\n' && rq[i-1]=='\r'); i++);
		rq[i-1]=0;
		printf("reqline %s\n", rq);
		h[j].n=hbuffer;
		for(i=0; i<1000 && read(s2, hbuffer+i,1);i++)
		{
			if(hbuffer[i]=='\n' && hbuffer[i-1]=='\r')
			{
				hbuffer[i-1]=0;
				if(h[j].n[0]==0) break;
				h[++j].n=hbuffer+i+1;
			}
			else if(hbuffer[i]==':' && !h[j].v)
			{
				hbuffer[i]=0;
				h[j].v=hbuffer+i+1;
			}
		}
		for(i=0; i<j; i++)
			printf("header: %s , value: %s\n", h[i].n, h[i].v);
		for(i=0; i<100 && rq[i]!=' '; i++);
		rq[i]=0;
		method=rq;
		filename=rq+i+1;
		for(; i<100 && rq[i]!=' '; i++);
		rq[i]=0;
		version=rq+i+1;
		char ch;
		printf("method: %s, filename: %s,version: %s\n",method, filename, version);
		if(!strcmp(method, "GET"))
		{
			if(!strncmp(filename, "/CGI/", 5))
			{
				sprintf(command,"%s > tmp.out", filename+5);
				system(command);
				sprintf(filename,"/tmp.out");
			}
			if((f=fopen(filename+1, "rt"))==NULL) {sprintf(response, "HTTP/1.1 404 not found\r\n\r\n"); write(s2,response,strlen(response));}
			else{
				int chunk_size=10;
				char bufferChunk[chunk_size+1];
				char* carriage="\r\n";
				bzero(bufferChunk,11);
				char* dim="A\r\n";
				char* lastChunk="0\r\n";
				char lastSize[4];//\r\nTransfer-Encoding: chunked
				sprintf(response, "HTTP/1.1 200 Ok\r\nTransfer-Encoding: chunked\r\n\r\n");
				write(s2,response, strlen(response));
				i=0;
				while(EOF!=(ch=fgetc(f)))
				{
					bufferChunk[i]=ch;
					i++;
					if(i==10)
					{
						write(s2,dim,strlen(dim));
				//		write(s2,carriage,2);
						write(s2, bufferChunk, 10);
						write(s2,carriage,2);
						i=0;
						bzero(bufferChunk, 11);
					}
				}
				if(i>0 && i<10)
				{
					sprintf(lastSize,"%d\r\n", i);
					write(s2, lastSize, strlen(lastSize));
					//write(s2,carriage,2);
					write(s2,bufferChunk,strlen(bufferChunk));
					write(s2,carriage,2);
				}
				write(s2,lastChunk,strlen(lastChunk));
				//write(s2,carriage,2);
				write(s2,carriage,2);
				fclose(f);
                        	printf("Finish response\n");
			}
		}	
		close(s2);
	}
}
