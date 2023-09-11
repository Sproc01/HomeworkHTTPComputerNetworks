#include <stdio.h>
#include <math.h>

/*int pow1(int x, int e)
{
        int z=1;
        for(int i=0; i<e; i++)
                z=z*x;
        return z;
}*/

long htonl(long a)
{
    int v[8];
    int r=a;
    for(int y=0;y<7; y++)
        {
                v[y]=r%2;
                r=r/2;
        }
    int k=0;
    for(int y=0; y<7; y++)
    {
        k+=pow(2,y)*v[y];
    }
    char* p=(char*)&a;
    if(p[0]!=k)
        return a;
    else
    {
        char temp=p[0];
        p[0]=p[3];
        p[3]=temp;
        temp=p[1];
        p[1]=p[2];
        p[2]=temp;
        return a;
    }
}

short int htons(short int a)
{
    int v[8];
    int r=a;
    for(int y=0;y<7; y++)
        {
                v[y]=r%2;
                r=r/2;
        }
    int k=0;
    for(int y=0; y<7; y++)
    {
        k+=pow(2,y)*v[y];
    }
    char* p=(char*)&a;
    if(p[0]!=k)
        return a;
    else
    {
        char temp = p[0];
        p[0] = p[1];
        p[1] = temp;
        return a;
    }
}

int main()
{
    short int a=1;
    printf("%d\n",htons(a));

    long b=1;
    printf("%ld\n",htonl(b));

}
