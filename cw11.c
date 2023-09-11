#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#define ENTITY_BODY_SIZE 10000000

struct header
{       
        char* n;
        char* v;
};


char* request="GET / HTTP/1.1\r\nHOST:www.google.com\r\n\r\n";
int s,i,t,j,l=-1;
int n,dim=0;
struct sockaddr_in c;
char sl[501];
char hbuf[10000];
struct header h[100];
char entity[ENTITY_BODY_SIZE+1];
char chunk[4];

int main()
{
      if(-1==(s=socket(AF_INET,SOCK_STREAM,0)))
        {
                perror("Socket Failed: ");
                printf("%d\n",errno);
                return -1;
        }
        c.sin_family=AF_INET;
        c.sin_port=htons(80);
        char* ip=&c.sin_addr.s_addr;
        ip[0]=142;
        ip[1]=250;
        ip[2]=180;
        ip[3]=132;
        t=connect(s,(struct sockaddr*)&c,sizeof(struct sockaddr_in));
        if(t==-1)
        {
                perror("Connect failed: ");
                printf("%d\n",errno);
        }
        t=write(s,request,strlen(request));

        for(i=0; i<500 && read(s,sl+i,1) && sl[i]!='\n';i++);
        sl[i]=0;
        printf("%s\n",sl);
        j=0;  
        h[j].n=hbuf;
        for(i=0;i<5001 && read(s,hbuf+i,1);i++)
        {
                if(hbuf[i]=='\n' && hbuf[i-1]=='\r')
                {
                        if(*(h[j].n)=='\r') break;
                        h[++j].n=hbuf+i+1;
                        hbuf[i-1]=0;
                }
                if(hbuf[i]==':' && !h[j].v)
                {
                        h[j].v=hbuf+i+1;
                        hbuf[i]=0;
                }
        }
        for(i=0; i<j; i++)
            printf("h[%d]: %s --->%s\n",i,h[i].n,h[i].v);

        if(strcmp(h[i-1].n,"content-length")==0)
            l=atoi(h[i].v);
        if(l!=-1)
                for(i=0; i<ENTITY_BODY_SIZE && (n=read(s,entity+i,ENTITY_BODY_SIZE-i));i+=n);
        else
        {
                while(l)
                {
                    l=0;
                    for(j=0;j<4 && read(s,chunk+j,1);j++)
                        if(chunk[j]=='\r')
                            break;
                    for(i=0;i<j;i++)
                    {
                        if(chunk[i]>='0' && chunk[i]<='9')
                            l+=(chunk[i]-'0')*pow(16,j-1-i);
                        if(chunk[i]>='a' && chunk[i]<='f')
                            l+=(chunk[i]-'a'+10)*pow(16,j-1-i);
                        if(chunk[i]>='A' && chunk[i]<='F')
                            l+=(chunk[i]-'A'+10)*pow(16,j-1-i);
                    }
                    if(j==4)
                        read(s,chunk,2);
                    else
                        read(s,chunk,1);
                    for(i=0;i<l && (n=read(s,entity+dim+i,l-i));i+=n);
                    read(s,chunk+2,2);
                    dim+=l;
                }
        }
        entity[dim]=0;
        printf("%s\n",entity);
        return 0;
}
