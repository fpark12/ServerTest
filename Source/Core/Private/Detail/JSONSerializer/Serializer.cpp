#include "Public/Detail/ProtobufJSON.h"
#include "StringFormat.h"
//#include "Common.h"
//#include "Log.h"
#include <rapidjson/writer.h>
#include <rapidjson/reader.h>
#include <rapidjson/stringbuffer.h>
#include <stack>

namespace JSON
{
	void Serializer::WriteMessage(google::protobuf::Message const& value)
	{
		google::protobuf::Reflection const* reflection = value.GetReflection();
		std::vector<google::protobuf::FieldDescriptor const*> fields;
		reflection->ListFields(value, &fields);

		Writer.StartObject();
		for (std::size_t i = 0; i < fields.size(); ++i)
		{
			WriteMessageField(value, fields[i]);
		}

		Writer.EndObject();
	}

	void Serializer::WriteMessageField(google::protobuf::Message const& value, google::protobuf::FieldDescriptor const* field)
	{
		Writer.Key(field->name().c_str());
		if (field->is_repeated())
		{
			Writer.StartArray();
			WriteRepeatedMessageField(value, field);
			Writer.EndArray();
		}
		else
			WriteSimpleMessageField(value, field);
	}

	void Serializer::WriteSimpleMessageField(google::protobuf::Message const& value, google::protobuf::FieldDescriptor const* field)
	{
		google::protobuf::Reflection const* reflection = value.GetReflection();
		switch (field->cpp_type())
		{
		case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
			WriteInt32(reflection->GetInt32(value, field));
			break;
		case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
			WriteInt64(reflection->GetInt64(value, field));
			break;
		case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
			WriteUInt32(reflection->GetUInt32(value, field));
			break;
		case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
			WriteUInt64(reflection->GetUInt64(value, field));
			break;
		case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
			WriteDouble(reflection->GetDouble(value, field));
			break;
		case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
			WriteFloat(reflection->GetFloat(value, field));
			break;
		case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
			WriteBool(reflection->GetBool(value, field));
			break;
		case google::protobuf::FieldDescriptor::CPPTYPE_ENUM:
			WriteEnum(reflection->GetEnum(value, field));
			break;
		case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
		{
			std::string strValue = reflection->GetString(value, field);
			if (field->type() == google::protobuf::FieldDescriptor::TYPE_STRING)
				WriteString(strValue);
			else
			{
				Writer.StartArray();
				for (std::size_t i = 0; i < strValue.length(); ++i)
					WriteUInt32(uint32(strValue[i]));
				Writer.EndArray();
			}
			break;
		}
		case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
			WriteMessage(reflection->GetMessage(value, field));
			break;
		default:
			break;
		}
	}

	void Serializer::WriteRepeatedMessageField(google::protobuf::Message const& value, google::protobuf::FieldDescriptor const* field)
	{
		google::protobuf::Reflection const* reflection = value.GetReflection();
		for (int32 i = 0; i < reflection->FieldSize(value, field); ++i)
		{
			switch (field->cpp_type())
			{
			case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
				WriteInt32(reflection->GetRepeatedInt32(value, field, i));
				break;
			case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
				WriteInt64(reflection->GetRepeatedInt64(value, field, i));
				break;
			case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
				WriteUInt32(reflection->GetRepeatedUInt32(value, field, i));
				break;
			case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
				WriteUInt64(reflection->GetRepeatedUInt64(value, field, i));
				break;
			case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
				WriteDouble(reflection->GetRepeatedDouble(value, field, i));
				break;
			case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
				WriteFloat(reflection->GetRepeatedFloat(value, field, i));
				break;
			case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
				WriteBool(reflection->GetRepeatedBool(value, field, i));
				break;
			case google::protobuf::FieldDescriptor::CPPTYPE_ENUM:
				WriteEnum(reflection->GetRepeatedEnum(value, field, i));
				break;
			case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
			{
				std::string strValue = reflection->GetRepeatedString(value, field, i);
				if (field->type() == google::protobuf::FieldDescriptor::TYPE_STRING)
					WriteString(strValue);
				else
				{
					Writer.StartArray();
					for (std::size_t j = 0; j < strValue.length(); ++j)
						WriteUInt32(uint32(strValue[j]));
					Writer.EndArray();
				}
				break;
			}
			case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
				WriteMessage(reflection->GetRepeatedMessage(value, field, i));
				break;
			default:
				break;
			}
		}
	}
}