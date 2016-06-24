#pragma once
#include "ChatMessage.h"
#include "ChatParticipant.h"
#include "ChatRoom.h"
#include "asio.hpp"

//----------------------------------------------------------------------

class ChatSession
	: public ChatParticiant,
	public std::enable_shared_from_this<ChatSession>
{
public:
	ChatSession(asio::ip::tcp::socket socket, ChatRoom& room);

	void start();

	void Deliver(const ChatMessage& msg);

private:
	void do_read_header();

	void do_read_body();

	void do_write();

	asio::ip::tcp::socket TcpSocket;
	ChatRoom& RoomRef;
	ChatMessage MessageRead;
	ChatMessageQueue WriteMessages;
};