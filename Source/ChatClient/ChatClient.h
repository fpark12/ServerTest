#pragma once
#include "asio.hpp"
#include "Core/Public/ChatMessage.h"


class chat_client
{
public:
	chat_client(asio::io_service& io_service,
		asio::ip::tcp::resolver::iterator endpoint_iterator);

	void write(const ChatMessage& msg);

	void close();

private:
	void do_connect(asio::ip::tcp::resolver::iterator endpoint_iterator);

	void do_read_header();

	void do_read_body();

	void do_write();

private:
	asio::io_service& io_service_;
	asio::ip::tcp::socket socket_;
	ChatMessage read_msg_;
	ChatMessageQueue write_msgs_;
};
