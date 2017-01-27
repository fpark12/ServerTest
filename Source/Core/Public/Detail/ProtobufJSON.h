#pragma once
#include <google/protobuf/message.h>
#include <google/protobuf/util/json_util.h>
#include <rapidjson/writer.h>
#include <rapidjson/reader.h>
#include <rapidjson/stringbuffer.h>

namespace Protobuf
{
	using google::protobuf::util::Status;

	Core_API google::protobuf::util::Status MessageToJson(google::protobuf::Message const& Message, std::string& OutString);
	Core_API google::protobuf::util::Status JsonToMessage(std::string const& OutString, google::protobuf::Message& Message);
}
// Currently using google's internal implementation instead
/*
namespace JSON
{
	Core_API std::string Serialize(google::protobuf::Message const& message);
	Core_API bool Deserialize(std::string json, google::protobuf::Message* message);

	class Core_API Serializer
	{
	public:
		Serializer() : Writer(Buffer) { }

		void WriteInt32(int32 value) { Writer.Int(value); }
		void WriteInt64(int64 value) { Writer.Int64(value); }
		void WriteUInt32(uint32 value) { Writer.Uint(value); }
		void WriteUInt64(uint64 value) { Writer.Uint64(value); }
		void WriteDouble(double value) { Writer.Double(value); }
		void WriteFloat(float value) { Writer.Double(value); }
		void WriteBool(bool value) { Writer.Bool(value); }
		void WriteEnum(google::protobuf::EnumValueDescriptor const* value) { Writer.String(value->name().c_str()); }
		void WriteString(std::string const& value) { Writer.String(value.c_str()); }
		void WriteMessage(google::protobuf::Message const& value);

		std::string GetString() const { return std::string(Buffer.GetString(), Buffer.GetSize()); }

	private:
		void WriteMessageField(google::protobuf::Message const& value, google::protobuf::FieldDescriptor const* field);
		void WriteSimpleMessageField(google::protobuf::Message const& value, google::protobuf::FieldDescriptor const* field);
		void WriteRepeatedMessageField(google::protobuf::Message const& value, google::protobuf::FieldDescriptor const* field);

		rapidjson::StringBuffer Buffer;
		rapidjson::Writer<rapidjson::StringBuffer> Writer;
	};


	class Core_API Deserializer : public rapidjson::BaseReaderHandler<rapidjson::UTF8<>, Deserializer>
	{
	public:
		bool ReadMessage(std::string json, google::protobuf::Message* message);

		bool Key(const Ch* str, rapidjson::SizeType length, bool copy);
		bool Null();
		bool Bool(bool b);
		bool Int(int32 i);
		bool Uint(uint32 i);
		bool Int64(int64 i);
		bool Uint64(uint64 i);
		bool Double(double d);
		bool String(const Ch* str, rapidjson::SizeType length, bool copy);
		bool StartObject();
		bool EndObject(rapidjson::SizeType memberCount);
		bool StartArray();
		bool EndArray(rapidjson::SizeType memberCount);

		std::vector<std::string> const& GetErrors() const { return _errors; }

	private:
		bool CheckType(google::protobuf::FieldDescriptor::CppType expectedType);

		rapidjson::Reader _reader;
		std::stack<google::protobuf::FieldDescriptor const*> _state;
		std::stack<google::protobuf::Message*> _objectState;
		std::vector<std::string> _errors;
	};
}
*/