#include <stdio.h>
#include <string.h>

#include "lua.hpp"
#include "crt32.h"

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
    printf("try to luaL_openlibs:!\n");
    luaL_openlibs(L);
    const char* lua_s = "io.write('Hello world, from ',_VERSION,'!\n')";

    // while (fgets(buff, sizeof(buff), stdin) != NULL)
    // {
    	printf("try to luaL_loadbuffer\n");
       error = luaL_loadbuffer(L, lua_s, strlen(lua_s), "line") || lua_pcall(L, 0, 0, 0);
       if (error) 
       {
           printf("%s", lua_tostring(L, -1));
           lua_pop(L, 1);/* pop error message from the stack */
       }
    // }

    lua_close(L);
    return 123;
}