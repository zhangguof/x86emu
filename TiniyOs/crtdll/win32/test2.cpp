#include <iostream>
#include <string>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define EXPORT __declspec(dllexport)
#define IMPORT __declspec(dllimport)
#define WINAPI __attribute__((__stdcall__))

struct Interface
{
	virtual void* get_fun_ptr(const char* name)=0;
	virtual int get_fun_num() = 0;
};

extern "C" {
	IMPORT void* get_func_by_name(const char* name);
	EXPORT int test_cpp();

	typedef Interface* f_get_interface_t();
	EXPORT void test_link(f_get_interface_t* ptr);
	
	WINAPI uint32_t DllMain(void* hinstDLL, uint32_t fdwReason, uint32_t lpvReserved);
}


WINAPI uint32_t DllMain(void* hinstDLL, uint32_t fdwReason, uint32_t lpvReserved)
{
	// printf("test2 DLLMain:%p,%d,%d\n",hinstDLL,fdwReason,lpvReserved);
	puts("test2 DllMain");
	return 1;
}

// int main()
// {
// 	std::string h = "hhh";
// 	return 0;
// }

// #include <unordered_map>
// std::unordered_map<std::string,int> s_map;
// class TestObj
// {
// 	public:
// 		TestObj()
// 		{
// 			printf("new TestObj!");
// 		}
// 	void test(){
// 		printf("do test!\n");
// 	}
// };

// TestObj testobj;



Interface* g_interface = nullptr;

void test_link(f_get_interface_t* ptr)
{
	g_interface = ptr();
	printf("get_interface:%p\n",g_interface);

	void* t = g_interface->get_fun_ptr("ggggg");
	int n = g_interface->get_fun_num();

	printf("get t:%p\n",t);
	printf("get n:%d\n",n);
	((void(*)())t)();


}

struct TestObj
{
	int a;
	int b;
	const char* name;
};
TestObj _obj;

EXPORT TestObj* get_test_obj()
{
	_obj.a = 1000;
	_obj.b = 2000;
	_obj.name = "hello world";
	return &_obj;
}


double f(double d)
{
	printf("in fffff!\n");
    return d > 1e7 ? throw std::overflow_error("too big") : d;
}
struct TestCls
{
	int a;
	int *n;
	char name[10];
	char ** names;
	void check()
	{
		printf("%p,%p,%p,%p\n",a,n,name,names);
		// printf("%d,%d,%s,%s\n",a,*n,name,names[0]);
	}
};

typedef void (*testf_t)(int a,int* b,const char* c,char** names);
typedef struct TestCls* (*testf_t2)(struct TestCls* p);

EXPORT int test_cpp()
{
	// std::cout<<"hello world!"<<std::endl;
	// std::string h = "std cout:hello world";
	// std::string name = "ttt";
	// std::cout<<h<<std::endl;
	// char * buf = new char[1024];
	// strcpy(buf,h.c_str());
	// printf("buf;:::%s\n",buf);
	// delete[] buf;
	const char* name = "wrap_test";
	void* p = get_func_by_name(name);
	printf("get f:%p\n",p);
	int a = 123;
	int *b = &a;
	const char* c = "hello !!1";
	char x[]="hello world!";
	// char* names = nullptr;
	char* names[3];
	int n = 0;

	((testf_t)p)(a,&n,c,names);
	printf("a=%d,c=%s,n=%d,%p\n",a,c,n,names);
	for(int i=0;i<n;++i)
	{
		printf("%d:%s,%p\n",i,names[i],names[i]);
	}


	p = get_func_by_name("wrap_test2");

	struct TestCls t = {
		.a=112233,
		.n = &n,
		.names = names
	};
	printf("size:%u,p:%p\n",sizeof(TestCls),p);
printf("names:%p,%p\n",names,t.names);
	auto ret = ((testf_t2)p)(&t);
	printf("off:%u,%u,%u,%u\n",&TestCls::a,&TestCls::n,
	&TestCls::name,&TestCls::names);
	printf("names:%p,%p\n",names,t.names);
	printf("%p,%p,%p\n",&t.n,t.names,t.names[0]);


	printf("%d,%d,%s\n",t.a,*(t.n),t.names[0]);
	printf("buf:%s,%p\n",t.name,t.name);
	t.check();
	printf("==========================\n");

	printf("ret === :%d,%s\n",ret->a,ret->name);
	ret->check();

	// try {
 //        std::cout << f(1e10) << '\n';
 //    } catch (const std::overflow_error& e) {
 //        std::cout << e.what() << '\n';
 //    }
	// s_map[name] = 1024;
	// printf("%s:%d\n",name.c_str(),s_map[name]);
	// std::cout<<h<<std::endl;
	// printf("%s\n",h.c_str());
	// puts(h.c_str());
	// printf("hello world\n");
	// fputc('X',stdout);
	// testobj.test();

	return 0;
}