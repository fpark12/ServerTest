#include "Public/Detail/ProtobufJSON.h"

namespace Protobuf
{
	bool MessageToJson(google::protobuf::Message const& Message, std::string& OutString)
	{
		return Protobuf::Status::OK == google::protobuf::util::MessageToJsonString(Message, &OutString);
	}

	bool JsonToMessage(std::string const& OutString, google::protobuf::Message& Message)
	{
		return Protobuf::Status::OK == google::protobuf::util::JsonStringToMessage(OutString, &Message);
	}
}
