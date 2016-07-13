#pragma once



#ifdef LUAPB_EXPORT
	#define LUAPB_API extern "C" _declspec(dllexport)
#else

	#define LUAPB_API extern "C" _declspec(dllimport)
#endif


LUAPB_API void luaopen_luapb(lua_State* L);