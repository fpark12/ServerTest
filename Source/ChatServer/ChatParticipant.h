#pragma once
#include <memory>
#include "Core/Public/Detail/ChatMessage.h"


class ChatParticiant
{
public:
	virtual ~ChatParticiant()
	{
	}
	virtual void Deliver(const ChatMessage& msg) = 0;
};

typedef std::shared_ptr<ChatParticiant> ChatParticipantPtr;