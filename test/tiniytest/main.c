char *str = "hello world!";

extern int add(int,int);
int _start()
{
	int a=10;
	int b=20;
	int c = add(a,b);
	__asm__("hlt");
	return 0;
}
