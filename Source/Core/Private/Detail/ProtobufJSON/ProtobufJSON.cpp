#include "Public/Detail/ProtobufJSON.h"

namespace Protobuf
{
	google::protobuf::util::Status MessageToJson(google::protobuf::Message const& Message, std::string& OutString)
	{
		return google::protobuf::util::MessageToJsonString(Message, &OutString);
	}

	google::protobuf::util::Status JsonToMessage(std::string const& OutString, google::protobuf::Message& Message)
	{
		return google::protobuf::util::JsonStringToMessage(OutString, &Message);
	}
}
