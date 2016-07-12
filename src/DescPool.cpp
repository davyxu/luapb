#include "stdafx.h"
#include "Utility.h"

class PBProtoErrorCollector : public google::protobuf::DescriptorPool::ErrorCollector
{
public:
	PBProtoErrorCollector()
	{

	}

	void AddError(const std::string& filename, const std::string& element_name, const google::protobuf::Message* descriptor, google::protobuf::DescriptorPool::ErrorCollector::ErrorLocation location, const std::string& message)
	{
		char buffer[10240];
		sprintf(buffer, "%s: %s ", filename.c_str(), message.c_str());

		mStream << buffer << "\r\n";
	}

	std::string GetLog(){ return mStream.str(); }

private:

	std::stringstream mStream;
};

google::protobuf::DescriptorPool* _pool = nullptr;

typedef std::map<std::string, const google::protobuf::FileDescriptor*> FileDescMap;
FileDescMap _filedescMap;

typedef std::map<std::string, const google::protobuf::Descriptor*> MessageMap;
MessageMap _msgMap;

typedef std::map<std::string, const google::protobuf::EnumDescriptor*> EnumMap;
EnumMap _enumMap;

const google::protobuf::Descriptor* FindMessage(const char* name)
{
	auto it = _msgMap.find(name);
	if (it != _msgMap.end())
	{
		return it->second;
	}

	return nullptr;
}

const google::protobuf::EnumDescriptor* FindEnum(const char* name)
{
	auto it = _enumMap.find(name);
	if (it != _enumMap.end())
	{
		return it->second;
	}

	return nullptr;
}



bool InitDescriptorPool( const char* filename )
{
	if (_pool != nullptr)
	{
		return false;
	}

	_pool = new google::protobuf::DescriptorPool;

	google::protobuf::FileDescriptorSet fds;
	if (!LoadDescriptorFile(filename, fds))
	{
		return false;
	}


	PBProtoErrorCollector pec;

	for (int i = 0; i < fds.file_size();i++)
	{
		auto fdp = fds.file(i);

		const google::protobuf::FileDescriptor* fileD = _pool->BuildFileCollectingErrors(fdp, &pec);

		if (fileD == NULL)
		{
			printf("%s", pec.GetLog().c_str());

			return false;
		}

		const google::protobuf::Descriptor* d = fileD->FindMessageTypeByName("Person");

		for (int i = 0; i < fileD->message_type_count();i++)
		{
			auto msg = fileD->message_type(i);
			_msgMap[msg->full_name()] = msg;
		}

		for (int i = 0; i < fileD->enum_type_count(); i++)
		{
			auto e = fileD->enum_type(i);
			_enumMap[e->full_name()] = e;
		}
		
	}

	return true;
}



void ReleaseDescriptorPool()
{
	if (_pool != nullptr)
	{
		_msgMap.clear();
		_enumMap.clear();

		delete _pool;
		_pool = nullptr;
	}
}