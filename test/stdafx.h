// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#include <stdio.h>
#include <tchar.h>

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}


#include "LuaBridge.h"
#include "RefCountedObject.h"
#include "RefCountedPtr.h"


// TODO: reference additional headers your program requires here
