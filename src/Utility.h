#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/descriptor_database.h>

extern void* LoadFileContent(const char* filename, unsigned int& size);

extern bool LoadDescriptorFile(const char* filename, google::protobuf::FileDescriptorSet& fds);