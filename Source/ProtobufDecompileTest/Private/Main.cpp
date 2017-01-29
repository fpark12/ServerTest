#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor.pb.h>
#include <iostream>

int main() {
	google::protobuf::FileDescriptorProto fileProto;
	fileProto.ParseFromFileDescriptor(0);
	google::protobuf::DescriptorPool pool;
	const google::protobuf::FileDescriptor* desc =
		pool.BuildFile(fileProto);
	std::cout << desc->DebugString() << std::endl;
	return 0;
}