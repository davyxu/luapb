#include "stdafx.h"
#include "DescPool.h"
#include "Stream.h"
#include "Decoder.h"

luabridge::LuaRef RawDecode(lua_State* L, PBStream* stream, const google::protobuf::Descriptor* msgD)
{
	luabridge::LuaRef tab = luabridge::newTable(L);	

	for (;;)
	{
		auto fieldnumber = stream->ReadFieldNumber();
		if (fieldnumber == 0)
			break;

		auto fd = msgD->FindFieldByNumber(fieldnumber);

		// 协议变化?
		if (fd == nullptr) {
			continue;
		}

		if (fd->type() == google::protobuf::FieldDescriptor::TYPE_MESSAGE)
		{
			int limit;
			stream->BeginMessage(limit);

			auto value = RawDecode(L, stream, fd->message_type());

			stream->EndMessage(limit);


			if (fd->is_repeated())
			{
				auto list = tab[fd->name()];
				if (list.isTable())
				{
					list.append(value);
				}
				else
				{
					auto newList = luabridge::newTable(tab.state());
					newList.append(value);
					tab[fd->name()] = newList;
				}
			}
			else
			{
				tab[fd->name()] = value;
			}
			
		}
		else
		{
			stream->ReadValue(tab, fd);
			
		}

	}

	return tab;
}


int Decode(lua_State* L)
{
	const char* name = luaL_checkstring(L, 1);

	auto buff = luabridge::Stack<std::string>::get(L, 2);

	auto msgD = FindMessage(name);
	if (msgD == nullptr)
	{
		printf("message '%s' not found\n", name);
		return 0;
	}

	PBStream stream((const ::uint8*)buff.c_str(), buff.size());

	auto tab = RawDecode(L, &stream, msgD);

	luabridge::Stack<luabridge::LuaRef>::push(L, tab);

	return 1;
}
