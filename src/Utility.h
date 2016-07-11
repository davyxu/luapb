

extern void* LoadFileContent(const char* filename, unsigned int& size);

extern bool LoadDescriptorFile(const char* filename, google::protobuf::FileDescriptorSet& fds);