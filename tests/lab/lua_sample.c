#include <stdio.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

/* the Lua interpreter */

struct service {
	int service_id;
	char service_name[50];
};

static int lua_print(lua_State *L) {
	int i;
	int nargs = lua_gettop(L);

	 for (i=1; i <= nargs; ++i) {
		printf("LUA: %s\n",  lua_tostring(L, i));
    }	
    return 0;
    
}
static int lua_bartlby_service_set_status(lua_State *L) {
	int status;
	struct service * svc;


	svc=lua_touserdata(L, 1);
	status=lua_tonumber(L, 2);

	printf("SET_STATUS: Service Object: %s\n", svc->service_name);
	printf("SET_STATUS: Code: %d\n",status);

	lua_pushnumber(L, -123);
	return 1;
}

int main ( int argc, char *argv[] )
{
	int res;
	lua_State* L;
	

	struct service svc = {
		.service_id=322,
		.service_name="sssasdf"
	};

	/* initialize Lua */	
	L = luaL_newstate();

	/* load various Lua libraries */
	luaL_openlibs(L);

	lua_register(L, "bartlby_service_set_status", lua_bartlby_service_set_status);
	lua_register(L, "print", lua_print);



	/* run the script */
	luaL_dostring(L, "return dofile('sample.lua')");


	res = lua_tonumber(L, -1);
	printf("Script ended with %d\n", res);


	/* the function name */
	lua_getglobal(L, "bartlby_service_finish_hook");


	lua_pushlightuserdata(L, (void*)&svc );

	
	lua_newtable(L);
	
	lua_pushliteral(L, "service_id" );
	lua_pushnumber(L, svc.service_id );
	lua_settable(L, -3);  


	lua_pushliteral(L, "service_name" );
	lua_pushstring(L, svc.service_name );
	lua_settable(L, -3);  


		
	
	
	if(lua_pcall(L, 2, 1, 0) != 0 ) {
		
		printf("error running function `bartlby_service_finish_hook': %s\n", lua_tostring(L, -1));

	} else {
		/* get the result */	
		res = (int)lua_tonumber(L, -1);
		lua_pop(L, 1);
		printf("HOOK ended with %d\n", res);
	}


	/* print the result */
	


	


	/* cleanup Lua */
	lua_close(L);

	return 0;
}