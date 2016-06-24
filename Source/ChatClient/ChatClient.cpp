//
// chat_client.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>
#include "asio.hpp"
#include "ChatMessage.h"
#include "ChatClient.h"

chat_client::chat_client(asio::io_service& io_service, asio::ip::tcp::resolver::iterator endpoint_iterator) : io_service_(io_service),
socket_(io_service)
{
	do_connect(endpoint_iterator);
}

void chat_client::write(const ChatMessage& msg)
{
	io_service_.post(
		[this, msg]()
	{
		bool write_in_progress = !write_msgs_.empty();
		write_msgs_.push_back(msg);
		if (!write_in_progress)
		{
			do_write();
		}
	});
}

void chat_client::close()
{
	io_service_.post([this]()
	{
		socket_.close();
	});
}

void chat_client::do_connect(asio::ip::tcp::resolver::iterator endpoint_iterator)
{
	asio::async_connect(socket_, endpoint_iterator,
		[this](std::error_code ec, asio::ip::tcp::resolver::iterator)
	{
		if (!ec)
		{
			do_read_header();
		}
	});
}

void chat_client::do_read_header()
{
	asio::async_read(socket_,
		asio::buffer(read_msg_.data(), ChatMessage::header_length),
		[this](std::error_code ec, std::size_t /*length*/)
	{
		if (!ec && read_msg_.decode_header())
		{
			do_read_body();
		} else
		{
			socket_.close();
		}
	});
}

void chat_client::do_read_body()
{
	asio::async_read(socket_,
		asio::buffer(read_msg_.body(), read_msg_.body_length()),
		[this](std::error_code ec, std::size_t /*length*/)
	{
		if (!ec)
		{
			std::cout.write(read_msg_.body(), read_msg_.body_length());
			std::cout << "\n";
			do_read_header();
		} else
		{
			socket_.close();
		}
	});
}

void chat_client::do_write()
{
	asio::async_write(socket_,
		asio::buffer(write_msgs_.front().data(),
		write_msgs_.front().length()),
		[this](std::error_code ec, std::size_t /*length*/)
	{
		if (!ec)
		{
			write_msgs_.pop_front();
			if (!write_msgs_.empty())
			{
				do_write();
			}
		} else
		{
			socket_.close();
		}
	});
}
