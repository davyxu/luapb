// test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../src/stdafx.h"
#include "../src/Encoder.h"
#include "../src/DescPool.h"
#include "addressbook.pb.h"

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

bool TestBytes(int index, std::string mem )
{
	std::string answer;

	switch (index)
	{
	case 1:

		tutorial::Person p;

		if (p.ParseFromString(mem))
		{
			if (p.name() != "hello")
			{
				printf("'name' not match");
				return false;
			}
			if (p.id() != 123456)
			{
				printf("'id' not match");
				return false;
			}

			if (p.test(0) != 1)
			{
				printf("'test1' not match");
				return false;
			}
			if (p.test(1) != 2)
			{
				printf("'test1' not match");
				return false;
			}

			return true;
		}

		break;
	}

	

	return false;
}

void luaopen_test(lua_State* L)
{
	luabridge::getGlobalNamespace(L)
		.addFunction("TestBytes", TestBytes);		
}

int _tmain(int argc, _TCHAR* argv[])
{
	InitDescriptorPool("game.pb");

	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	
	luaopen_luapb(L);
	luaopen_test(L);

	


	dofile(L, "test.lua");


	lua_close(L);

	ReleaseDescriptorPool();

	return 0;
}

