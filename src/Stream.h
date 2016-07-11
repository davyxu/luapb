#pragma once

enum PBStreamUsage
{
	PBU_Write = 0,		// 脚本使用, 确定大小buffer直接写入
	PBU_ZeroCopyRead,		// C++使用, 不拷贝的数据, 直接读取
};


class PBStream
{
public:
	// 脚本使用, 确定大小buffer直接写入
	PBStream(uint32 Size);

	// C++使用, 不拷贝的数据, 直接读取
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
