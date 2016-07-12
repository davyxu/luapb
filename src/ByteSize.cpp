#include "stdafx.h"
#include "Stream.h"
#include "DescPool.h"

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

		// TODO 将每个字段按FieldNumber缓冲起来, 保存到Encode时使用, 提高效率

		if (fieldD->type() == google::protobuf::FieldDescriptor::TYPE_MESSAGE)
		{
			if (fieldD->is_repeated())
			{
				auto valueList = it.value();
				if (valueList.type() != LUA_TTABLE)
				{
					luaL_error(tab.state(), "repeated '%s' require table", name);
					return 0;
				}

				// 遍历列表
				for (luabridge::Iterator valueIt(it.value()); !valueIt.isNil(); ++valueIt)
				{
					size += PBStream::TagSize(fieldD->number());
					auto structSize = RawByteSize(fieldD->message_type(), valueIt.value());
					size += google::protobuf::io::CodedOutputStream::VarintSize32(structSize) + structSize;
				}
			}
			else
			{
				size += PBStream::TagSize(fieldD->number());
				size += RawByteSize(fieldD->message_type(), it.value());
			}

		}
		else
		{
			if (fieldD->is_repeated())
			{
				auto valueList = it.value();
				if (valueList.type() != LUA_TTABLE)
				{
					luaL_error(tab.state(), "repeated '%s' require table", name);
					return 0;
				}

				for (luabridge::Iterator valueIt(it.value()); !valueIt.isNil(); ++valueIt)
				{
					size += PBStream::TagSize(fieldD->number());
					size += PBStream::FieldSize(fieldD, valueIt.value());
				}
			}
			else
			{
				size += PBStream::TagSize(fieldD->number());
				size += PBStream::FieldSize(fieldD, it.value());
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
