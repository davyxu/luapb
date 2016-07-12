#include "stdafx.h"
#include "Stream.h"

PBStream::PBStream(uint32 Size)
	: mData(NULL)
	, mSize(0)
	, mDataReader(NULL)
	, mUsage(PBU_Write)
{
	mPointer = mData = new google::protobuf::uint8[Size];
	mSize = Size;
}

PBStream::PBStream(const uint8* Data, uint32 Size)
	: mData((uint8*)Data)
	, mSize(Size)
	, mUsage(PBU_ZeroCopyRead)
{
	using namespace google::protobuf;

	mDataReader = new io::CodedInputStream(reinterpret_cast<const google::protobuf::uint8*>(mData), mSize);
}

PBStream::~PBStream()
{
	if (mUsage == PBU_Write)
	{
		SafeDelete(mData);
	}

	SafeDelete(mDataReader);
}



// Tag + Variant
void PBStream::WriteValue(const google::protobuf::FieldDescriptor* fd, luabridge::LuaRef LuaValue)
{
	using namespace google::protobuf;

	switch (fd->type())
	{
	case FieldDescriptor::TYPE_FLOAT:
	{
		float Value = LuaValue;

		mPointer = internal::WireFormatLite::WriteFloatToArray(fd->number(), Value, mPointer);
	}
	break;
	case FieldDescriptor::TYPE_INT32:
	{		
		::int32 Value = LuaValue;
		mPointer = internal::WireFormatLite::WriteInt32ToArray(fd->number(), Value, mPointer);
	}
	break;
	case FieldDescriptor::TYPE_BOOL:
	{
		bool Value = LuaValue;
		mPointer = internal::WireFormatLite::WriteBoolToArray(fd->number(), Value, mPointer);
	}
	break;
	case FieldDescriptor::TYPE_STRING:
	case FieldDescriptor::TYPE_INT64:
	case FieldDescriptor::TYPE_UINT64:
	{
		const char* Value = LuaValue;

		int StrLen = strlen(Value);
		mPointer = internal::WireFormatLite::WriteTagToArray(fd->number(), internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED, mPointer);
		mPointer = google::protobuf::io::CodedOutputStream::WriteVarint32ToArray(StrLen, mPointer);
		mPointer = google::protobuf::io::CodedOutputStream::WriteRawToArray(Value, StrLen, mPointer);
	}
	break;
	case FieldDescriptor::TYPE_UINT32:
	{
		::uint32 Value = LuaValue;
		mPointer = internal::WireFormatLite::WriteUInt32ToArray(fd->number(), Value, mPointer);
	}
	break;
	case FieldDescriptor::TYPE_ENUM:
	{
		std::string Value = LuaValue;

		if (fd->enum_type() == nullptr)
		{
			printf("field is not enum: %s\n", fd->full_name());
			return;
		}

		auto evd = fd->enum_type()->FindValueByName(Value);
		if (evd == nullptr)
		{
			printf("enum value not exists: %s in %s\n", Value.c_str(), fd->enum_type()->full_name().c_str());
			return;
		}

		mPointer = internal::WireFormatLite::WriteEnumToArray(fd->number(), evd->number(), mPointer);
	}
	break;
	default:
		assert("unknown fd type" == 0);
	}
}

// Tag + Size
void PBStream::WriteMessageHead(int FieldNumber, int MessageBytes)
{
	using namespace google::protobuf;

	mPointer = internal::WireFormatLite::WriteTagToArray(FieldNumber, internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED, mPointer);

	mPointer = io::CodedOutputStream::WriteVarint32ToArray(MessageBytes, mPointer);
}

#define FillValue \
	if (fd->is_repeated()) \
	{ \
		auto list = msgTable[fd->name()]; \
		if (list.isTable()) \
		{ \
			list.append(Value); \
		} \
		else \
		{ \
			auto newList = luabridge::newTable(msgTable.state()); \
			newList.append(Value); \
			msgTable[fd->name()] = newList; \
		} \
	} \
	else \
	{ \
		msgTable[fd->name()] = Value; \
	}


void PBStream::ReadValue(luabridge::LuaRef msgTable, const google::protobuf::FieldDescriptor* fd )
{
	using namespace google::protobuf;	

	bool OK = false;

	switch (fd->type())
	{
	case FieldDescriptor::TYPE_FLOAT:
	{
		float Value = 0;
		OK = internal::WireFormatLite::ReadPrimitive<float, internal::WireFormatLite::TYPE_FLOAT>(mDataReader, &Value);
		FillValue
	}
	break;
	case FieldDescriptor::TYPE_INT32:
	{
		::int32 Value = 0;
		OK = internal::WireFormatLite::ReadPrimitive<google::protobuf::int32, internal::WireFormatLite::TYPE_INT32>(mDataReader, &Value);
		FillValue
	}
	break;
	case FieldDescriptor::TYPE_BOOL:
	{
		bool Value = false;
		OK = internal::WireFormatLite::ReadPrimitive<bool, internal::WireFormatLite::TYPE_BOOL>(mDataReader, &Value);
		FillValue
	}
	break;
	case FieldDescriptor::TYPE_STRING:
	case FieldDescriptor::TYPE_INT64:
	case FieldDescriptor::TYPE_UINT64:
	{
		std::string Value;
		OK = internal::WireFormatLite::ReadString(mDataReader, &Value);
		FillValue
	}
	break;
	case FieldDescriptor::TYPE_UINT32:
	{
		::uint32 Value = 0;
		OK = internal::WireFormatLite::ReadPrimitive<google::protobuf::uint32, internal::WireFormatLite::TYPE_UINT32>(mDataReader, &Value);
		FillValue
	}
	break;
	case FieldDescriptor::TYPE_ENUM:
	{
		::int32 Number = 0;
		OK = internal::WireFormatLite::ReadPrimitive<int, internal::WireFormatLite::TYPE_ENUM>(mDataReader, &Number);


		std::string Value;

		auto evd = fd->enum_type()->FindValueByNumber(Number);
		if (evd == nullptr)
		{
			printf("field is not enum: %s\n", fd->full_name());
			return;
		}

		Value = evd->name();

		FillValue
	}
	break;
	default:
		{
			printf("field type is not support: %s\n", fd->full_name());
		}
		break;
	}
}


bool PBStream::BeginMessage( int& limit )
{
	::int32 size = 0;

	using namespace google::protobuf;

	if (!internal::WireFormatLite::ReadPrimitive<google::protobuf::int32, internal::WireFormatLite::TYPE_INT32>(mDataReader, &size))
	{
		return false;
	}


	if (!mDataReader->IncrementRecursionDepth())
		return false;

	limit = mDataReader->PushLimit(size);

	return true;
}

bool PBStream::EndMessage(int limit )
{
	if (!mDataReader->ConsumedEntireMessage())
		return false;

	mDataReader->PopLimit(limit);

	mDataReader->DecrementRecursionDepth();

	return true;
}


int PBStream::ReadFieldNumber()
{
	assert(mDataReader != NULL);

	auto tag = mDataReader->ReadTag();
	if (tag == 0)
		return 0;

	return google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag);
}



int PBStream::FieldSize(const google::protobuf::FieldDescriptor* fd, luabridge::LuaRef LuaValue)
{
	using namespace google::protobuf;

	switch (fd->type())
	{
	case FieldDescriptor::TYPE_FLOAT:
	{
		return 4;
	}
	break;
	case FieldDescriptor::TYPE_INT64:
	{
		const char* StrValue = LuaValue;

		::int64 Value = 0;
		::sscanf(StrValue, "%lld", &Value);

		return internal::WireFormatLite::Int64Size(Value);
	}
	break;
	case FieldDescriptor::TYPE_UINT64:
	{
		const char* StrValue = LuaValue;

		::uint64 Value = 0;
		::sscanf(StrValue, "%llu", &Value);

		return internal::WireFormatLite::UInt64Size(Value);
	}
	break;
	case FieldDescriptor::TYPE_INT32:
	{
		::int32 Value = LuaValue;
		return internal::WireFormatLite::Int32Size(Value);
	}
	break;
	case FieldDescriptor::TYPE_BOOL:
	{
		return 1;
	}
	break;
	case FieldDescriptor::TYPE_STRING:
	{
		const char* Value = LuaValue;

		int StrLen = strlen(Value);
		return io::CodedOutputStream::VarintSize32(StrLen) + StrLen;
	}
	break;
	case FieldDescriptor::TYPE_UINT32:
	{
		::uint32 Value = LuaValue;
		return internal::WireFormatLite::UInt32Size(Value);
	}
	break;
	case FieldDescriptor::TYPE_ENUM:
	{
		std::string Value = LuaValue;

		auto evd = fd->enum_type()->FindValueByName(Value);
		if (evd == nullptr)
		{
			printf("enum value not exists: %s in %s", Value.c_str(), fd->enum_type()->full_name().c_str() );
			return 0;
		}

		return internal::WireFormatLite::EnumSize(evd->number());
	}
	break;
	case FieldDescriptor::TYPE_MESSAGE:
	{
		// 这里的消息由lua取出后在这里计算
		int MsgSize = LuaValue;
		return io::CodedOutputStream::VarintSize32(MsgSize) + MsgSize;
	}
	break;
	}

	assert("Unsupport type" == 0);

	return 0;
}

int PBStream::TagSize(int FieldNumber)
{
	using namespace google::protobuf;

	return io::CodedOutputStream::VarintSize32(FieldNumber << internal::WireFormatLite::kTagTypeBits);
}
