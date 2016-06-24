#include "asio.hpp"
#include "ChatSession.h"

void ChatSession::do_write()
{
	auto self(shared_from_this());
	asio::async_write(TcpSocket,
		asio::buffer(WriteMessages.front().data(),
		WriteMessages.front().length()),
		[this, self](std::error_code ec, std::size_t /*length*/)
	{
		if (!ec)
		{
			WriteMessages.pop_front();
			if (!WriteMessages.empty())
			{
				do_write();
			}
		} else
		{
			RoomRef.Leave(shared_from_this());
		}
	});
}

void ChatSession::do_read_body()
{
	auto self(shared_from_this());
	asio::async_read(TcpSocket,
		asio::buffer(MessageRead.body(), MessageRead.body_length()),
		[this, self](std::error_code ec, std::size_t /*length*/)
	{
		if (!ec)
		{
			RoomRef.deliver(MessageRead);
			do_read_header();
		} else
		{
			RoomRef.Leave(shared_from_this());
		}
	});
}

void ChatSession::do_read_header()
{
	auto self(shared_from_this());
	asio::async_read(TcpSocket,
		asio::buffer(MessageRead.data(), ChatMessage::header_length),
		[this, self](std::error_code ec, std::size_t /*length*/)
	{
		if (!ec && MessageRead.decode_header())
		{
			do_read_body();
		} else
		{
			RoomRef.Leave(shared_from_this());
		}
	});
}

void ChatSession::Deliver(const ChatMessage& msg)
{
	bool write_in_progress = !WriteMessages.empty();
	WriteMessages.push_back(msg);
	if (!write_in_progress)
	{
		do_write();
	}
}

void ChatSession::start()
{
	RoomRef.Join(shared_from_this());
	do_read_header();
}

ChatSession::ChatSession(asio::ip::tcp::socket socket, ChatRoom& room) :
	TcpSocket(std::move(socket)),
	RoomRef(room)
{

}

