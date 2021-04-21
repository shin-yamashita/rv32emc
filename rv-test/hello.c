#include <stdio.h>
int main(void) 
{
 char *msg = "Hello world!\n";
 int i;

 for(i = 0; i < 5; i++)
   printf("%d: %s", i, msg);
 return 0; 
}
