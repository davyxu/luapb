#include "Utility.h"

#include <stdio.h>

void* LoadFileContent(const char* filename, unsigned int& size)
{
	FILE *f = fopen(filename, "rb");

	if (f == NULL)
	{
		return NULL;
	}

	fseek(f, 0, SEEK_END);
	size = ftell(f);

	//文件指针恢复到文件头位置
	fseek(f, 0, SEEK_SET);

	char* buff = (char*) new char[sizeof(char)*size];
	if (buff == NULL)
		return NULL;

	fread(buff, sizeof(char), size, f);
	fclose(f);

	return buff;
}

bool LoadDescriptorFile(const char* filename, google::protobuf::FileDescriptorProto& fdp)
{
	unsigned int size = 0;
	auto content = LoadFileContent(filename, size);


	if (!fdp.ParseFromArray(content, size))
	{
		delete content;
		return false;
	}

	delete content;
	return true;

}
