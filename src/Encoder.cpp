extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "LuaBridge.h"
#include "RefCountedObject.h"
#include "RefCountedPtr.h"



#include "encoder.h"


void encode(const char* name, luabridge::LuaRef tab)
{
	for (luabridge::Iterator it(tab); !it.isNil(); ++it)
	{
		const char* name = it.key();
		int value = it.value();
		int a = 1;
	}
}


void luaopen_luapb(lua_State* L ){

	luabridge::getGlobalNamespace(L).addFunction("encode", encode);
}