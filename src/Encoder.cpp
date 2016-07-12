#include "stdafx.h"
#include "DescPool.h"
#include "Stream.h"
#include "ByteSize.h"
#include "Encoder.h"

#include "Decoder.h"


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
					stream->WriteMessageHead(fieldD->number(), RawByteSize(fieldD->message_type(), valueIt.value()));
					RawEncode(stream, fieldD->message_type(), valueIt.value());
				}
			}
			else
			{
				stream->WriteMessageHead(fieldD->number(), RawByteSize(fieldD->message_type(), it.value()));
				RawEncode(stream, fieldD->message_type(), it.value());
			}
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
					stream->WriteValue(fieldD, valueIt.value());
				}
			}
			else
			{
				stream->WriteValue(fieldD, it.value());
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
		.addCFunction("Encode", Encode)
		.addCFunction("Decode", Decode);
}