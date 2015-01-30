/* $Id$ */
/* ----------------------------------------------------------------------- *
 *
 *   Copyright 2005-2009 Helmut Januschka - All Rights Reserved
 *   Contact: <helmut@januschka.com>, <contact@bartlby.org>
 *
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, Inc., 675 Mass Ave, Cambridge MA 02139,
 *   USA; either version 2 of the License, or (at your option) any later
 *   version; incorporated herein by reference.
 *
 *   visit: www.bartlby.org for support
 * ----------------------------------------------------------------------- */
/*
$Revision$
$HeadURL$
$Date$
$Author$ 


SAMPLE LUA SCRIPT:
*  if main script returns <0 no hook will be fired
* if you need a larger script do
		return dofile('/path/to/script/script.lua')

function bartlby_service_finish_hook(svc_obj, svc_table) 
	print("######## HOOK CALLED - Start")
	print("# service_id: " ..  svc_table["service_id"])
	print("# current_output: " ..  svc_table["current_output"])
	print("# current_state: " ..  svc_table["current_state"])
	print("#	   setting new state to 4")
	r = bartlby_service_set_status(svc_obj, 4)
	print("#    call returned RES:" .. r)
	r = bartlby_service_set_output(svc_obj, "new output")
	print("#    call returned RES:" .. r)
	print("######## HOOK CALLED - END")
	return r
end
return 1




*/




#include <bartlby.h>


#ifndef LUA_ADDON
int bartlby_finish_script(struct service * svc, char * script){
	return 0;
} 
#else
int bartlby_lua_finish_hook(struct service * svc,char * script, lua_State *L) {

	int script_return;
	int hook_end = -1;
	/* run the script svc->lua_script */
	if(luaL_dostring(L, script) != 0) {
		_log(LH_LUA, B_LOG_CRIT, "LUA Failed to call script %s", script);
		return -1;
	}


	script_return = lua_tonumber(L, -1);

	if(script_return < 0) {
		return script_return;
	} 
	
	lua_getglobal(L, "bartlby_service_finish_hook");
	lua_pushlightuserdata(L, (void*)svc );
	lua_newtable(L);
	lua_pushliteral(L, "service_id" );
	lua_pushnumber(L, svc->service_id );
	lua_settable(L, -3);  
	lua_pushliteral(L, "current_output" );
	lua_pushstring(L, svc->current_output );
	lua_settable(L, -3);  
	lua_pushliteral(L, "current_state" );
	lua_pushnumber(L, svc->current_state );
	lua_settable(L, -3);  


		
	
	
	if(lua_pcall(L, 2, 1, 0) != 0 ) {
		_log(LH_LUA, B_LOG_CRIT, "LUA Failed to call `bartlby_service_finish_hook`: %s",lua_tostring(L, -1));
	} else {
		/* get the result */	
		hook_end = (int)lua_tonumber(L, -1);
		lua_pop(L, 1);
		_log(LH_LUA, B_LOG_DEBUG, "Hook Ended with: %d",hook_end);
	}
	return hook_end;
}

static int lua_print(lua_State *L) {
	int i;
	int nargs = lua_gettop(L);

	 for (i=1; i <= nargs; ++i) {
	 	_log(LH_LUA, B_LOG_DEBUG, "LUA OUT: %s",lua_tostring(L, i));
    }	
    return 0;
    
}


static int lua_bartlby_service_set_output(lua_State *L) {
	
	struct service * svc;
	const char * output;

	

	svc=lua_touserdata(L, 1);
	output=lua_tostring(L, 2);


	if(svc == NULL) {
		lua_pushnumber(L, -1);
		return 1;
	}
	if(output == NULL) {
		lua_pushnumber(L, -1);
		return 1;
	}
	snprintf(svc->current_output,1024, "%s", output);
		
	lua_pushnumber(L, 1);
	return 1;
}

static int lua_bartlby_service_set_status(lua_State *L) {
	int status;
	struct service * svc;

	

	svc=lua_touserdata(L, 1);
	status=lua_tonumber(L, 2);

	if(svc != NULL) {
		switch(status) {
			case STATE_OK:
			case STATE_WARNING:
			case STATE_CRITICAL:
				svc->current_state=status;
			break;
			default:
				svc->current_state=STATE_UNKOWN;
			break;
		}
	}
		
	
	lua_pushnumber(L, svc->current_state);
	return 1;
}


lua_State * bartlby_lua_init() {
	lua_State* L;
	
	/* initialize Lua */	
	L = luaL_newstate();

	/* load various Lua libraries */
	luaL_openlibs(L);

	lua_register(L, "bartlby_service_set_status", lua_bartlby_service_set_status);
	lua_register(L, "bartlby_service_set_output", lua_bartlby_service_set_output);
	lua_register(L, "print", lua_print);


	return L;
}



void bartlby_lua_finish(lua_State * L) {
	lua_close(L);

}

int bartlby_finish_script(struct service * svc, char * script) {
	int rtc;
	lua_State * L=bartlby_lua_init();
	rtc=bartlby_lua_finish_hook(svc,script, L);
	bartlby_lua_finish(L);
	return rtc;

}

#endif
