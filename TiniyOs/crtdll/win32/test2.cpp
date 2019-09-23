#include <iostream>
#include <string>
#include <stdio.h>
#include <stdint.h>

#define EXPORT __declspec(dllexport)
#define IMPORT __declspec(dllimport)
#define WINAPI __attribute__((__stdcall__))
extern "C" {
	EXPORT int test_cpp();
	WINAPI uint32_t DLLMain(void* hinstDLL, uint32_t fdwReason, uint32_t lpvReserved);
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


double f(double d)
{
	printf("in fffff!\n");
    return d > 1e7 ? throw std::overflow_error("too big") : d;
}


EXPORT int test_cpp()
{
	// std::cout<<"hello world!"<<std::endl;
	std::string h = "std cout:hello world";
	std::string name = "ttt";
	std::cout<<h<<std::endl;

	try {
        std::cout << f(1e10) << '\n';
    } catch (const std::overflow_error& e) {
        std::cout << e.what() << '\n';
    }
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