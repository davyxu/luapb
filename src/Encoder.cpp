#include "stdafx.h"
#include "Encoder.h"
#include "DescPool.h"
#include "Stream.h"

int RawByteSize(const google::protobuf::Descriptor* msgD, luabridge::LuaRef tab)
{
	int size = 0;

	for (luabridge::Iterator it(tab); !it.isNil(); ++it)
	{
		const char* name = it.key();

		auto fieldD = msgD->FindFieldByName(name);
		if (fieldD == nullptr)
		{
			continue;
		}

		// 将每个字段按FieldNumber缓冲起来, 保存到Encode时使用, 提高效率

		if (fieldD->type() == google::protobuf::FieldDescriptor::TYPE_MESSAGE)
		{
			size += PBStream::TagSize(fieldD->number());
			size += RawByteSize(fieldD->message_type(), it.value());
		}
		else
		{
			if (fieldD->is_repeated())
			{
				auto valueList = it.value();
				if (valueList.type() != LUA_TTABLE)
				{
					luaL_error(tab.state(), "repeated value '%s' require table", name);
					return 0;
				}

				for (luabridge::Iterator valueIt(it.value()); !valueIt.isNil(); ++valueIt)
				{
					size += PBStream::TagSize(fieldD->number());
					size += PBStream::FieldSize(fieldD->type(), valueIt.value());
				}
			}
			else
			{
				size += PBStream::TagSize(fieldD->number());
				size += PBStream::FieldSize(fieldD->type(), it.value());
			}
			
		}
	}

	return size;
}

int ByteSize(const char* name, luabridge::LuaRef tab)
{
	auto msgD = FindMessage(name);
	if (msgD == nullptr)
	{
		printf("message '%s' not found\n", name);
		return false;
	}

	return RawByteSize(msgD, tab);
}



void RawEncode(PBStream* stream, const google::protobuf::Descriptor* msgD, luabridge::LuaRef tab)
{
	for (luabridge::Iterator it(tab); !it.isNil(); ++it)
	{
		const char* name = it.key();

		auto fieldD = msgD->FindFieldByName(name);
		if (fieldD == nullptr)
		{
			continue;
		}

		if (fieldD->type() == google::protobuf::FieldDescriptor::TYPE_MESSAGE)
		{
			stream->WriteMessageHead(fieldD->number(), RawByteSize(fieldD->message_type(), it.value()));
			RawEncode(stream, fieldD->message_type(), it.value());
		}
		else
		{
			if (fieldD->is_repeated())
			{
				auto valueList = it.value();
				if (valueList.type() != LUA_TTABLE)
				{
					luaL_error(tab.state(), "repeated value '%s' require table", name);
					return;
				}

				for (luabridge::Iterator valueIt(it.value()); !valueIt.isNil(); ++valueIt)
				{
					stream->WriteValue(fieldD->number(), fieldD->type(), valueIt.value());
				}
			}
			else
			{
				stream->WriteValue(fieldD->number(), fieldD->type(), it.value());
			}
		}
	}
}


int Encode(lua_State* L )
{
	
	const char* name = luaL_checkstring(L, 1);

	auto tab = luabridge::Stack<luabridge::LuaRef>::get(L, 2);

	auto msgD = FindMessage(name);
	if (msgD == nullptr)
	{
		printf("message '%s' not found\n", name);
		return 0;
	}

	int size = RawByteSize(msgD, tab);

	PBStream stream( size );

	RawEncode(&stream, msgD, tab);

	lua_pushlstring(L, (const char*)stream.GetData(), stream.GetSize());

	return 1;
}




void luaopen_luapb(lua_State* L ){

	luabridge::getGlobalNamespace(L)
		.addFunction("ByteSize", ByteSize)
		.addCFunction("Encode", Encode);
}