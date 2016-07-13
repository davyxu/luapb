#pragma once

#include <sstream>
#include <stdio.h>
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/descriptor_database.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/wire_format.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}


#include "LuaBridge.h"
#include "RefCountedObject.h"
#include "RefCountedPtr.h"


// 跨平台类型
#ifdef _WIN32
typedef __int8  int8;
typedef __int16 int16;
typedef __int32 int32;
typedef __int64 int64;

typedef unsigned __int8  uint8;
typedef unsigned __int16 uint16;
typedef unsigned __int32 uint32;
typedef unsigned __int64 uint64;
#else
typedef int8_t  int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

#endif

#define SafeDelete(p) {if(p) {delete (p);(p)= 0 ;}}

#define max(a,b)    (((a) > (b)) ? (a) : (b))


#include "luapb.h"

extern bool InitDescriptorPool(const char* filename);

extern void ReleaseDescriptorPool();

extern int RawByteSize(const google::protobuf::Descriptor* msgD, luabridge::LuaRef tab);

extern const google::protobuf::Descriptor* FindMessage(const char* name);

extern const google::protobuf::EnumDescriptor* FindEnum(const char* name);