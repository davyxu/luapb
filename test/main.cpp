// test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../src/Encoder.h"
#include "../src/DescPool.h"

void dofile(lua_State* L, const char* filename )
{
	auto status = luaL_loadfile(L, filename);

	if (status == 0)
	{
		auto result = lua_pcall(L, 0, LUA_MULTRET, 0);
		if (result != 0)
		{
			printf("%s", lua_tostring(L, -1));
		}
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	InitDescriptorPool("game.pb");

	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	
	luaopen_luapb(L);	

	


	dofile(L, "test.lua");


	lua_close(L);

	ReleaseDescriptorPool();

	return 0;
}

