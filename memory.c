#include <stdio.h>
#include <stdlib.h>

char x='A';
char y='B';

void fun(int cont)
{
        if(cont==10)
            return;
        else
        {
                char s='y';
                char g='z';
                char* p=(char*)malloc(48);
                printf("cont:%d\n", cont);
                char* p1=&x;
                printf("global x: %ld\n", p1);
                p1=&y;
                printf("global y:%ld\n", p1);
                p1=&s;
                printf("local s:%ld\n", p1);
                p1=&g;
                printf("local g:%ld\n",p1);
                p1=p;
                printf("dynamic p:%ld\n", p1);
                printf("------------------------\n");
                fun(cont+1);
        }
}

int main()
{
    fun(0);
}
/*output
 cont:0
 global x: 6295632
 global y:6295636
 local s:140725290427670
 local g:140725290427671
 dynamic p:36372496
 ------------------------
 cont:1
 global x: 6295632
 global y:6295636
 local s:140725290427606
 local g:140725290427607
 dynamic p:36373600
 ------------------------
 cont:2
 global x: 6295632
 global y:6295636
 local s:140725290427542
 local g:140725290427543
 dynamic p:36373664
 ------------------------
 cont:3
 global x: 6295632
 global y:6295636
 local s:140725290427478
 local g:140725290427479
 dynamic p:36373728
 ------------------------
 cont:4
 global x: 6295632
 global y:6295636
 local s:140725290427414
 local g:140725290427415
 dynamic p:36373792
 ------------------------
 cont:5
 global x: 6295632
 global y:6295636
 local s:140725290427350
 local g:140725290427351
 dynamic p:36373856
 ------------------------
 cont:6
 global x: 6295632
 global y:6295636
 local s:140725290427286
 local g:140725290427287
 dynamic p:36373920
 ------------------------
 cont:7
 global x: 6295632
 global y:6295636
 local s:140725290427222
 local g:140725290427223
 dynamic p:36373984
 ------------------------
 cont:8
 global x: 6295632
 global y:6295636
 local s:140725290427158
 local g:140725290427159
 dynamic p:36374048
 ------------------------
 cont:9
 global x: 6295632
 global y:6295636
 local s:140725290427094
 local g:140725290427095
 dynamic p:36374112
 ------------------------
 
 */

