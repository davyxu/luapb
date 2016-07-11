#pragma once

enum PBStreamUsage
{
	PBU_Write = 0,		// �ű�ʹ��, ȷ����Сbufferֱ��д��
	PBU_ZeroCopyRead,		// C++ʹ��, ������������, ֱ�Ӷ�ȡ
};


class PBStream
{
public:
	// �ű�ʹ��, ȷ����Сbufferֱ��д��
	PBStream(uint32 Size);

	// C++ʹ��, ������������, ֱ�Ӷ�ȡ
	PBStream(const ::uint8* Data, ::uint32 Size);

	virtual ~PBStream();

	::uint8* GetData(){ return mData; }

	::uint32 GetSize(){ return mSize; }

	const ::uint8* ToString(){ return mData; }

public:
	void BeginParse();

	void EndParse();

	::uint32 ReadTag();

	bool MessageReadGuard(::uint32 MessageLength, luabridge::LuaRef UnmarshalFunc, luabridge::LuaRef NewMsg);

	int ReadValue(lua_State* L);

public:

	void WriteValue(int FieldNumber, int FieldType, luabridge::LuaRef Value);

	void WriteMessageHead(int FieldNumber, int MessageBytes);	

public:
	static int FieldSize(int FieldType, luabridge::LuaRef LuaValue);
	static int TagSize(int FieldNumber);
private:
	::uint32 mSize;
	::uint8* mData;
	::uint8* mPointer;
	PBStreamUsage mUsage;
	google::protobuf::io::CodedInputStream* mDataReader;
};
