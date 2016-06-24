#include "ChatRoom.h"

void ChatRoom::Join(ChatParticipantPtr participant)
{
	Participants.insert(participant);
	for (auto msg : RecentMessages)
	{
		participant->Deliver(msg);
	}
}

void ChatRoom::Leave(ChatParticipantPtr participant)
{
	Participants.erase(participant);
}

void ChatRoom::deliver(const ChatMessage& msg)
{
	RecentMessages.push_back(msg);
	while (RecentMessages.size() > max_recent_msgs)
		RecentMessages.pop_front();

	for (auto participant : Participants)
		participant->Deliver(msg);
}

