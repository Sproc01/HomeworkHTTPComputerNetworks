#define _XOPEN_SOURCE

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
/*
struct tm {
               int tm_sec;     Seconds (0-60) 
               int tm_min;     Minutes (0-59) 
               int tm_hour;    Hours (0-23) 
               int tm_mday;    Day of the month (1-31)
               int tm_mon;     Month (0-11)
               int tm_year;    Year - 1900
               int tm_wday;    Day of the week (0-6, Sunday = 0)
               int tm_yday;    Day in the year (0-365, 1 Jan = 0)
               int tm_isdst; 
           };
*/


struct header{
	char* n;
	char* v;
};


int s,t,k;
char date[100];
time_t tm;
struct tm cachetm;
struct tm headertm;
char* res="www.example.com";
struct sockaddr_in server;
char sl[100];
char buffer[2000];
struct header h[500];
char response[10000000];
char ch;
int flag=0;

int main()
{
	char request[100];
	char request2[100];
	char path[100];
	sprintf(request,"GET / HTTP/1.1\r\nHOST:%s\r\n\r\n",res);
	sprintf(request2,"HEAD / HTTP/1.1\r\nHOST:%s\r\n\r\n",res);
	FILE* f;
	s=socket(AF_INET, SOCK_STREAM, 0);
	if(s==-1){perror("Socket error: "); printf("%d\n", errno); return -1;}
	server.sin_family=AF_INET;
	server.sin_port=htons(80);
	char* ip=(unsigned char*)&server.sin_addr.s_addr;
	ip[0]=93;
	ip[1]=184;
	ip[2]=216;
	ip[3]=34;
	t=connect(s, (const struct sockaddr*)&server, sizeof(struct sockaddr_in));
	if(t==-1){perror("Connect error: "); printf("%d\n",errno); return -1;}
	sprintf(path,"cache/%s",res);
	f=fopen(path,"rt");
	if(f!=NULL)
	{
		int i=0;
		while((ch=fgetc(f))!='\r')
		{
			date[i]=ch;
			i++;
		}
		write(s,request2, strlen(request2));
		fclose(f);
		k=1;
		strptime(date,"%a,%t%d%t%b%t%Y%t%T",&cachetm);
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
		for(int i=0;i<j;i++)
        	{	
                	printf("Header: %s, value: %s\n", h[i].n, h[i].v);
               		if(!(strcmp(h[i].n,"Last-Modified")))
                        {
				strptime(h[i].v,"%t%a,%t%d%t%b%t%Y%t%T%tGMT", &headertm);
				int t=mktime(&headertm);
                		int t1=mktime(&cachetm);
                		printf("%d>%d?\n",t1,t);
               			if(t1>t)
                		{
                        		f=fopen(path, "rt");
                        		while(EOF!=(ch=fgetc(f)))
                                		printf("%c",ch);
                        		printf("\nFinish response\n");
                        		return 0;
                		}
        		}
		}
	}
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
		if(!(strcmp(h[i].n,"Expires")))
			if(!(strcmp(h[i].n,"-1")))
				flag=-1;	
	}
	if(L!=-1)
	{	
		t=0;
		printf("Response:\n");
		for(int i=0; i<100000 && (t=read(s, response+i, L-i)); i+=t)
		printf("%s", response);
		if(flag!=-1)
		{
			f=fopen(path,"w");
			tm=time(NULL);
			cachetm=*localtime(&tm);
			strftime(date,100,"%a, %d %b %Y %T\r\n",&cachetm);
			fwrite(date,1,strlen(date),f);
			fwrite(response,1,strlen(response),f);
			fclose(f);
		}
		printf("\n");
	}
	else
	{	
		if(flag!=-1)
		{
			f=fopen(path,"w");
			tm=time(NULL);
			cachetm=*localtime(&tm);
			strftime(date,100,"%a, %d %b %Y %T\r\n",&cachetm);
			fwrite(date,1,strlen(date),f);
			fclose(f);
		}
		while(L!=0)
		{
			char chunksize[10];
			int i=0;
			for(i=0;i<10 && read(s, chunksize+i,1) && !(chunksize[i]=='\n' && chunksize[i-1]=='\r');i++);
			chunksize[i]=0;
			L=(int)strtol(chunksize, NULL, 16);
			for(int i=0; i<1000000 && (t=read(s, response+i,L-i));i+=t);
			printf("%s",response);
			if(flag!=-1)
			{
				f=fopen(path,"a");
				fwrite(response,1,strlen(response),f);
				fclose(f);
			}
			printf("\n");
			read(s,chunksize,2);
			if(chunksize[0]!='\r' || chunksize[1]!='\n'){printf("Chunk error"); return -1;}
		}
		printf("End response\n");
	}
}
