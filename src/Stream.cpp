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
	, mDataReader(NULL)
	, mUsage(PBU_ZeroCopyRead)
{

}

PBStream::~PBStream()
{
	if (mUsage == PBU_Write)
	{
		SafeDelete(mData);
	}

	SafeDelete(mDataReader);
}



void PBStream::WriteValue(int FieldNumber, int FieldType, luabridge::LuaRef LuaValue)
{
	using namespace google::protobuf;

	switch (FieldType)
	{
	case FieldDescriptor::TYPE_FLOAT:
	{
		float Value = LuaValue;

		mPointer = internal::WireFormatLite::WriteFloatToArray(FieldNumber, Value, mPointer);
	}
	break;
	case FieldDescriptor::TYPE_INT32:
	{
		::int32 Value = LuaValue;
		mPointer = internal::WireFormatLite::WriteInt32ToArray(FieldNumber, Value, mPointer);
	}
	break;
	case FieldDescriptor::TYPE_BOOL:
	{
		bool Value = LuaValue;
		mPointer = internal::WireFormatLite::WriteBoolToArray(FieldNumber, Value, mPointer);
	}
	break;
	case FieldDescriptor::TYPE_STRING:
	case FieldDescriptor::TYPE_INT64:
	case FieldDescriptor::TYPE_UINT64:
	{
		const char* Value = LuaValue;

		int StrLen = strlen(Value);
		mPointer = internal::WireFormatLite::WriteTagToArray(FieldNumber, internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED, mPointer);
		mPointer = google::protobuf::io::CodedOutputStream::WriteVarint32ToArray(StrLen, mPointer);
		mPointer = google::protobuf::io::CodedOutputStream::WriteRawToArray(Value, StrLen, mPointer);
	}
	break;
	case FieldDescriptor::TYPE_UINT32:
	{
		::uint32 Value = LuaValue;
		mPointer = internal::WireFormatLite::WriteUInt32ToArray(FieldNumber, Value, mPointer);
	}
	break;
	case FieldDescriptor::TYPE_ENUM:
	{
		::int32 Value = LuaValue;
		mPointer = internal::WireFormatLite::WriteEnumToArray(FieldNumber, Value, mPointer);
	}
	break;
	default:
		assert("unknown fd type" == 0);
	}
}


void PBStream::WriteMessageHead(int FieldNumber, int MessageBytes)
{
	using namespace google::protobuf;

	mPointer = internal::WireFormatLite::WriteTagToArray(FieldNumber, internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED, mPointer);

	mPointer = io::CodedOutputStream::WriteVarint32ToArray(MessageBytes, mPointer);
}


int PBStream::ReadValue(lua_State* L)
{
	using namespace google::protobuf;

	FieldDescriptor::Type FieldType = (FieldDescriptor::Type)luaL_checkinteger(L, 2);

	bool OK = false;

	switch (FieldType)
	{
	case FieldDescriptor::TYPE_FLOAT:
	{
		float Value = 0;
		OK = internal::WireFormatLite::ReadPrimitive<float, internal::WireFormatLite::TYPE_FLOAT>(mDataReader, &Value);
		luabridge::push(L, Value);
	}
	break;
	case FieldDescriptor::TYPE_INT32:
	{
		::int32 Value = 0;
		OK = internal::WireFormatLite::ReadPrimitive<google::protobuf::int32, internal::WireFormatLite::TYPE_INT32>(mDataReader, &Value);
		luabridge::push(L, Value);
	}
	break;
	case FieldDescriptor::TYPE_BOOL:
	{
		bool Value = false;
		OK = internal::WireFormatLite::ReadPrimitive<bool, internal::WireFormatLite::TYPE_BOOL>(mDataReader, &Value);
		luabridge::push(L, Value);
	}
	break;
	case FieldDescriptor::TYPE_STRING:
	case FieldDescriptor::TYPE_INT64:
	case FieldDescriptor::TYPE_UINT64:
	{
		std::string Value;
		OK = internal::WireFormatLite::ReadString(mDataReader, &Value);
		luabridge::push(L, Value);
	}
	break;
	case FieldDescriptor::TYPE_UINT32:
	{
		::uint32 Value = 0;
		OK = internal::WireFormatLite::ReadPrimitive<google::protobuf::uint32, internal::WireFormatLite::TYPE_UINT32>(mDataReader, &Value);
		luabridge::push(L, Value);
	}
	break;
	case FieldDescriptor::TYPE_ENUM:
	{
		::int32 Value = 0;
		OK = internal::WireFormatLite::ReadPrimitive<int, internal::WireFormatLite::TYPE_ENUM>(mDataReader, &Value);
		luabridge::push(L, Value);
	}
	break;
	case FieldDescriptor::TYPE_MESSAGE:
	{
		::uint32 Length;
		OK = mDataReader->ReadVarint32(&Length);
		luabridge::push(L, Length);
	}
	break;
	default:
		assert("unknown fd type" == 0);
	}

	luabridge::push(L, OK);

	return 2;
}


bool PBStream::MessageReadGuard(uint32 MessageLength, luabridge::LuaRef UnmarshalFunc, luabridge::LuaRef NewMsg)
{
	if (!mDataReader->IncrementRecursionDepth())
		return false;

	int Limit = mDataReader->PushLimit(MessageLength);

	UnmarshalFunc(NewMsg, this);

	if (!mDataReader->ConsumedEntireMessage())
		return false;

	mDataReader->PopLimit(Limit);

	mDataReader->DecrementRecursionDepth();

	return true;
}

void PBStream::BeginParse()
{
	using namespace google::protobuf;

	assert(mDataReader == NULL);

	mDataReader = new io::CodedInputStream(reinterpret_cast<const google::protobuf::uint8*>(mData), mSize);
}

void PBStream::EndParse()
{
	using namespace google::protobuf;

	assert(mDataReader != NULL);

	SafeDelete(mDataReader);
}

uint32 PBStream::ReadTag()
{
	assert(mDataReader != NULL);

	return mDataReader->ReadTag();
}



int PBStream::FieldSize(int FieldType, luabridge::LuaRef LuaValue)
{
	using namespace google::protobuf;

	switch (FieldType)
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
		::int32 Value = LuaValue;
		return internal::WireFormatLite::EnumSize(Value);
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
