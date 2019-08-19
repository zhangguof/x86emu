char *str = "hello world!";

extern int add(int,int);
extern int get_val();
int _start()
{
	int a=10;
	int b=20;
	int c = add(a,b);
	c += get_val();
	__asm__("hlt");
	return 0;
}
