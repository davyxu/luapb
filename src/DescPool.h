#pragma once


extern bool InitDescriptorPool(const char* filename);

extern void ReleaseDescriptorPool();

const google::protobuf::Descriptor* FindMessage(const char* name);
const google::protobuf::EnumDescriptor* FindEnum(const char* name);