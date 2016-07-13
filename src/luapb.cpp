#include "stdafx.h"
#include "luapb.h"

extern int ByteSize(const char* name, luabridge::LuaRef tab);
extern int Encode(lua_State* L);
extern int Decode(lua_State* L);

void luaopen_luapb(lua_State* L){	

	luabridge::getGlobalNamespace(L)
		.addCFunction("luapb_encode", Encode)
		.addCFunction("luapb_decode", Decode)
		.addFunction("luapb_register", InitDescriptorPool);
}