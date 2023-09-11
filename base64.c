#include <stdio.h>
#include <math.h>
#include <stdlib.h>

unsigned int t,i,k,z;
unsigned char c,c1;
unsigned short int x;
char s[900];
char res[1200];
char* p;
char string[3];
int cont;

int conversionascii(char d)
{
	if(d>0 && d<26)
        	return d+65;
	else if(d>25 && d<52)
		return d+97-26;
	else if(d>51 && d<62)
		return d-52+48;
	else if(d==62)
		return '+';
	else
		return '/';
}


void conversion()
{	
	int h=0;
	while(s[h])
	{
		for(int j=0;j<3 && s[h];j++)
			string[j]=s[h++];
		p=&k;
        	for(i=0;string[i];i++)
			p[3-i]=string[i];
		i=k>>2;
		p=&i;
        	res[cont++]=conversionascii(p[3]);
        	for(i=0;i<3;i++)
        	{
        		k=k<<6;
                	z=k>>2;
                	p=&z;
                	res[cont++]=conversionascii(p[3]);
       		}
	}
}


int main()
{
	scanf("%s",s);
        printf("%s\n",s);
	for(t=0;s[t];t++);
	if(t%3==0)
		conversion();
	else if(t%3==1)
	{
		c=s[t-1];
		s[t-1]=0;
		conversion();
		c1=c>>2;
		res[cont++]=conversionascii(c1);
		c1=c<<6;
		c1=c1>>2;
		res[cont++]=conversionascii(c1);
		res[cont++]='=';
		res[cont++]='=';
	}
	else 
	{
		p=&x;
		p[0]=s[t-1];
		p[1]=s[t-2];
		s[t-2]=0;
		conversion();
		z=x>>2;
		p=&z;
		res[cont++]=conversionascii(p[1]);
		for(int v=0;v<2;v++)
		{
			x=x<<6;
			z=x>>2;
			res[cont++]=conversionascii(p[1]);
		}
		res[cont++]='=';
	}
	res[cont]=0;
	printf("%s\n",res);
}
