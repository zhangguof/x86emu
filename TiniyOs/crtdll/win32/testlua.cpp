#include <stdio.h>
#include <string.h>

#include "lua.hpp"
// #include "crt32.h"
#define EXPORT __declspec(dllexport)

extern "C" 
{  
    EXPORT int testlua();
} 

int testlua()
{
    char buff[256];
    int error;
    printf("new lua_State!\n");
    lua_State *L = luaL_newstate(); // 创建Lua虚拟机
    // luaopen_base(L);

    // printf("after luaopen_base!!!!\n");
    // printf("try to luaL_openlibs:!\n");
    
    luaL_openlibs(L);

    // const char* lua_s = "io.write('Hello world, from ',_VERSION,'!\n')";
    const char* lua_s = "print('Lua5.3 Hello world')";

    // while (fgets(buff, sizeof(buff), stdin) != NULL)
    // {
    	printf("try to luaL_loadbuffer\n");
       error = luaL_loadbuffer(L, lua_s, strlen(lua_s), "line") || lua_pcall(L, 0, 0, 0);
       if (error) 
       {
       	printf("====error:::::\n");
           printf("%s", lua_tostring(L, -1));
           lua_pop(L, 1);/* pop error message from the stack */
       }
    // }
       printf("lua before close!\n");
    // lua_close(L);
    return 123;
}

int main()
{
	testlua();
	return 0;
}
