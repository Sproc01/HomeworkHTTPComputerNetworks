#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <signal.h>

struct header{
	char* n;
	char* v;
};

int s,s2,s3,i,j,t;
struct header h[100];
char *method,*uri,*version;
char hbuffer[100000];
char buffer[1000];
char response[1000];
char request[1000];
char rq[100];
struct sockaddr_in local, client, server;
struct hostent* host;
char *hostname;
char *port;
char response2[1000];
char request2[1000];
char* filename;

int main()
{
	if((s=socket(AF_INET, SOCK_STREAM,0))==-1){perror("Socket error: "); printf("%d", errno); return -1;}
	local.sin_family=AF_INET;
	local.sin_port=htons(8081);
	int len=sizeof(struct sockaddr_in);
	if((t=bind(s,(const struct sockaddr*)&local,len))==-1){perror("Bind error: "); printf("%d", errno); return -1;}
	if((t=listen(s, 20))==-1){perror("Listen error: "); printf("%d", errno); return -1;}
	while(1)
	{
		j=0;
		i=0;
		bzero(hbuffer, 100000);
		bzero(response,1000);
		bzero(request,1000);
		bzero(rq, 100);
		s2=accept(s, (struct sockaddr*)&client, &len);
		if(fork()) continue;
		if(s2==-1){perror("Accept error: "); printf("%d", errno); return -1;}
		for(i=0;i<100 && read(s2, rq+i,1) && !(rq[i]=='\n' && rq[i-1]=='\r'); i++);
		rq[i-1]=0;
		printf("Reqline: %s\n", rq);
		h[j].n=hbuffer;
		for(i=0; i<100000 && read(s2,hbuffer+i,1);i++)
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
			printf("Header: %s, value: %s\n", h[i].n, h[i].v);
		
		method=rq;
		for(i=0; i<100 && rq[i]!=' '; i++);
		rq[i++]=0;
		uri=rq+i;
		for(; i<100 && rq[i]!=' '; i++);
		rq[i++]=0;
		version=rq+i;
		printf("method: %s, uri: %s, version: %s\n", method, uri, version);
		if(!(strcmp(method, "GET")))
		{//get http://www.google.com/file/file
            
            for(i=0; uri[i] && uri[i]!=':';i++);
            uri[i]=0;
            i++;
            if(uri[i]!='/' || uri[i+1]!='/'){printf("Errore parsing url\n"); return -1;}
            i+=2;
            hostname=uri+i;
            for(; uri[i] && uri[i]!='/';i++);
            uri[i]=0;
            filename=uri+i+1;
            host=gethostbyname(hostname);
            server.sin_family=AF_INET;
            server.sin_port=htons(80);
            unsigned char* ip=(unsigned char*)&server.sin_addr.s_addr;
            ip[0]=host->h_addr[0];
            ip[1]=host->h_addr[1];
            ip[2]=host->h_addr[2];
            ip[3]=host->h_addr[3];
            s3=socket(AF_INET, SOCK_STREAM,0);
            if(s3==-1){perror("Socket server error: "); printf("%d", errno); return -1;}
            if(-1==connect(s3,(const struct sockaddr*)&server, sizeof(struct sockaddr_in)))
            {
                perror("connect server error: "); printf("%d", errno); return -1;
            }
            sprintf(response, "HTTP/1.1 200 ok\r\n\r\n");
            write(s2, response, strlen(response));
            sprintf(request, "GET /%s HTTP/1.1\r\nHOST:%s\r\nConnection:Close\r\n\r\n",filename, hostname);
            write(s3, request,strlen(request));
            char ch;
            while((t=read(s3, buffer, 1000)))
                write(s2,buffer,t);
            close(s3);
            
		}
		else if(!(strcmp(method, "CONNECT")))
		{
			hostname=uri;
			for(i=0; uri[i] && uri[i]!=':'; i++);
			uri[i++]=0;
			port=uri+i;
			host=gethostbyname(hostname);
			printf("hostname:%s, port: %s\n", hostname,port);			
			server.sin_family=AF_INET;
			server.sin_port=htons(atoi(port));
			unsigned char *ip=(unsigned char*)&server.sin_addr.s_addr;
			ip[0]=host->h_addr[0];
			ip[1]=host->h_addr[1];
			ip[2]=host->h_addr[2];
			ip[3]=host->h_addr[3];
			printf("ip: %d.%d.%d.%d\n", ip[0],ip[1], ip[2], ip[3]);
			if((s3=socket(AF_INET, SOCK_STREAM,0))==-1){perror("Socket server error: "); printf("%d", errno); return -1;}
			len=sizeof(struct sockaddr_in);
			if((t=connect(s3, (const struct sockaddr*)&server,len))==-1){perror("connect server error: "); printf("%d", errno); return -1;}
			sprintf(response, "HTTP/1.1 200 Establihsed\r\n\r\n");
			write(s2, response, strlen(response));
			int pid=0;
			if(pid=fork())
			{//padre	
				while((t=read(s3, response2, 1000)))
					write(s2, response2, t);
				kill(pid, SIGTERM);
				close(s3);
			}
			else
			{//figlio
				while((t=read(s2, request2, 1000)))
					write(s3,request2, t);
				exit(0);
			}
			

		}
		close(s2);
        exit(1);
	}
	close(s);
}
