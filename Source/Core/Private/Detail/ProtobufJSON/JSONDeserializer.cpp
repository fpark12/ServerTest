#include "Public/Detail/ProtobufJSON.h"

namespace JSON
{
	bool Deserializer::ReadMessage(std::string json, google::protobuf::Message* message)
	{
		rapidjson::StringStream ss(json.c_str());

		_objectState.push(message);

		rapidjson::ParseResult result = _reader.Parse(ss, *this);

		ASSERT(result.IsError() || (_objectState.empty() && _state.empty()));

		return !result.IsError() && _errors.empty();
	}

	bool Deserializer::Key(const Ch* str, rapidjson::SizeType length, bool copy)
	{
		google::protobuf::FieldDescriptor const* field = _objectState.top()->GetDescriptor()->FindFieldByName(str);
		if (!field)
		{
			_errors.push_back(::String("Message %s has no field %s.", _objectState.top()->GetTypeName().c_str(), str));
			return false;
		}

		_state.push(field);
		return true;
	}

	bool Deserializer::Null()
	{
		_state.pop();
		return true;
	}

#define SET_FIELD(message, field, Type, val) do { \
    if (!field->is_repeated()) \
        message->GetReflection()->Set ## Type(message, field, val); \
    else \
        message->GetReflection()->Add ## Type(message, field, val); \
    _state.pop(); \
    } while (0)

	bool Deserializer::Bool(bool b)
	{
		if (!CheckType(google::protobuf::FieldDescriptor::CPPTYPE_BOOL))
			return false;

		SET_FIELD(_objectState.top(), _state.top(), Bool, b);
		return true;
	}

	bool Deserializer::Int(int32 i)
	{
		if (!CheckType(google::protobuf::FieldDescriptor::CPPTYPE_INT32))
			return false;

		SET_FIELD(_objectState.top(), _state.top(), Int32, i);
		return true;
	}

	bool Deserializer::Uint(uint32 i)
	{
		google::protobuf::FieldDescriptor const* field = _state.top();
		google::protobuf::Message* message = _objectState.top();
		switch (field->cpp_type())
		{
		case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
			SET_FIELD(message, field, UInt32, i);
			break;
		case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
		{
			if (field->type() != google::protobuf::FieldDescriptor::TYPE_BYTES)
			{
				_errors.emplace_back("Expected field type to be bytes but got string instead.");
				return false;
			}
			std::string currentValue = message->GetReflection()->GetString(*message, field);
			currentValue.append(1, (char)i);
			message->GetReflection()->SetString(message, field, currentValue);
			break;
		}
		default:
			_errors.push_back(::String("Expected field type to be uint32 or string but got %s instead.", _state.top()->cpp_type_name()));
			return false;
		}

		return true;
	}

	bool Deserializer::Int64(int64 i)
	{
		if (!CheckType(google::protobuf::FieldDescriptor::CPPTYPE_INT64))
			return false;

		SET_FIELD(_objectState.top(), _state.top(), Int64, i);
		return true;
	}

	bool Deserializer::Uint64(uint64 i)
	{
		if (!CheckType(google::protobuf::FieldDescriptor::CPPTYPE_UINT64))
			return false;

		SET_FIELD(_objectState.top(), _state.top(), UInt64, i);
		return true;
	}

	bool Deserializer::Double(double d)
	{
		google::protobuf::FieldDescriptor const* field = _state.top();
		google::protobuf::Message* message = _objectState.top();
		switch (field->cpp_type())
		{
		case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
			SET_FIELD(message, field, Float, float(d));
			break;
		case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
			SET_FIELD(message, field, Double, d);
			break;
		default:
			_errors.push_back(::String("Expected field type to be float or double but got %s instead.", _state.top()->cpp_type_name()));
			return false;
		}

		return true;
	}

	bool Deserializer::String(const Ch* str, rapidjson::SizeType length, bool copy)
	{
		google::protobuf::FieldDescriptor const* field = _state.top();
		google::protobuf::Message* message = _objectState.top();
		switch (field->cpp_type())
		{
		case google::protobuf::FieldDescriptor::CPPTYPE_ENUM:
		{
			google::protobuf::EnumValueDescriptor const* enumValue = field->enum_type()->FindValueByName(str);
			if (!enumValue)
			{
				_errors.push_back(::String("Field %s enum %s does not have a value named %s.", field->full_name().c_str(), field->enum_type()->full_name().c_str(), str));
				return false;
			}

			SET_FIELD(message, field, Enum, enumValue);
			break;
		}
		case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
			SET_FIELD(message, field, String, str);
			break;
		default:
			_errors.push_back(::String("Expected field type to be string or enum but got %s instead.", _state.top()->cpp_type_name()));
			return false;
		}

		return true;
	}

	bool Deserializer::StartObject()
	{
		// not a root object
		if (!_state.empty())
		{
			if (_state.top()->cpp_type() != google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE)
			{
				_errors.push_back(::String("Expected field %s to be a message but got %s instead.", _state.top()->cpp_type_name()));
				return false;
			}

			google::protobuf::Message* containingMessage = _objectState.top();
			if (!_state.top()->is_repeated())
				_objectState.push(containingMessage->GetReflection()->MutableMessage(containingMessage, _state.top()));
			else
				_objectState.push(containingMessage->GetReflection()->AddMessage(containingMessage, _state.top()));
		}
		else if (_objectState.size() != 1)
			return false;

		return true;
	}

	bool Deserializer::EndObject(rapidjson::SizeType memberCount)
	{
		if (!_state.empty() && !_state.top()->is_repeated())
			_state.pop();

		_objectState.pop();
		return true;
	}

	bool Deserializer::StartArray()
	{
		if (_state.empty())
		{
			_errors.emplace_back("Root cannot be an array.");
			return false;
		}

		if (_state.top()->is_repeated() ^ (_state.top()->type() != google::protobuf::FieldDescriptor::TYPE_BYTES))
		{
			_errors.push_back(::String("Expected field %s type to be exactly an array OR bytes but it was both or none.", _state.top()->full_name().c_str()));
			return false;
		}

		return true;
	}

	bool Deserializer::CheckType(google::protobuf::FieldDescriptor::CppType expectedType)
	{
		if (_state.top()->cpp_type() != expectedType)
		{
			_errors.push_back(::String("Expected field %s type to be %s but got %s instead.",
				_state.top()->full_name().c_str(), google::protobuf::FieldDescriptor::CppTypeName(expectedType), _state.top()->cpp_type_name()));
			return false;
		}

		return true;
	}

	bool Deserializer::EndArray(rapidjson::SizeType memberCount)
	{
		_state.pop();
		return true;
	}

#undef SET_FIELD

	std::string JSON::Serialize(google::protobuf::Message const& message)
	{
		Serializer serializer;
		serializer.WriteMessage(message);
		return serializer.GetString();
	}

	bool JSON::Deserialize(std::string json, google::protobuf::Message* message)
	{
		Deserializer deserializer;
		if (!deserializer.ReadMessage(std::forward<std::string>(json), message))
		{
			for (std::size_t i = 0; i < deserializer.GetErrors().size(); ++i)
			{
				//GLog.Critical("json", "%s", deserializer.GetErrors()[i].c_str());
			}
			return false;
		}

		return true;
	}

}