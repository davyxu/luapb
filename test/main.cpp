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

tutorial::Person answerPb;


bool TestEncode(int index, std::string mem )
{
	std::string answer;

	switch (index)
	{
	case 1:

		tutorial::Person memPb;
		if ( memPb.ParseFromString(mem) && 
			memPb.SerializeAsString() == answerPb.SerializeAsString())
		{
		
			return true;
		}

		break;
	}

	

	return false;
}

void luaopen_test(lua_State* L)
{
	luabridge::getGlobalNamespace(L)
		.addFunction("TestEncode", TestEncode);		
}

int _tmain(int argc, _TCHAR* argv[])
{

	
	answerPb.set_name("hello");
	answerPb.set_id(123456);
	answerPb.add_test(1);
	answerPb.add_test(2);
	
		{
		auto number = answerPb.add_phone();
		number->set_number("789");
		number->set_type(tutorial::WORK);
		}

		{
		auto number = answerPb.add_phone();
		number->set_number("456");
		number->set_type(tutorial::HOME);
		}
	
	

	auto size = answerPb.ByteSize();

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

