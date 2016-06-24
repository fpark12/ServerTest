#pragma once
#include <set>
#include <queue>
#include "ChatMessage.h"
#include "ChatParticipant.h"

//----------------------------------------------------------------------

class ChatRoom
{
public:
	void Join(ChatParticipantPtr participant);

	void Leave(ChatParticipantPtr participant);

	void deliver(const ChatMessage& msg);

private:
	std::set<ChatParticipantPtr> Participants;
	enum
	{
		max_recent_msgs = 100
	};

	ChatMessageQueue RecentMessages;
};

