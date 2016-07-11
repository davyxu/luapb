

#include "Utility.h"

google::protobuf::SimpleDescriptorDatabase* _pool = nullptr;



bool InitDescriptorPool( const char* filename )
{
	if (_pool != nullptr)
	{
		return false;
	}

	_pool = new google::protobuf::SimpleDescriptorDatabase;

	google::protobuf::FileDescriptorProto fdp;
	if (!LoadDescriptorFile(filename, fdp))
	{
		return false;
	}

	if (!_pool->Add(fdp))
	{
		return false;
	}
	google::protobuf::FileDescriptorProto fdp2;
	_pool->FindFileContainingSymbol("tutorial.Person", &fdp2);


	return true;
}

void ReleaseDescriptorPool()
{
	if (_pool != nullptr)
	{
		delete _pool;
		_pool = nullptr;
	}
}