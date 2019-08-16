#include <stdio.h>
extern int add(int,int);
//rdi,rsi,rdx,rcx,r8,r9

int main()
{
	int a=10;
	int b=20;
	int c = add(a,b);
	printf("%d\n",c);

	return 0;
}