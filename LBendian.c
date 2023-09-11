#include <stdio.h>

int main()
{
    short int a=1;
    char* p=(char*)&a;
    if(*p==1)
        printf("Little Endian");
    else
        printf("Big Endian");
}