#pragma once

int RawByteSize(const google::protobuf::Descriptor* msgD, luabridge::LuaRef tab);

int ByteSize(const char* name, luabridge::LuaRef tab);