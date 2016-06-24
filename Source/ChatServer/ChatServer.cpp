//
// chat_server.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "ChatServer.h"
#include "ChatSession.h"
//----------------------------------------------------------------------

ChatServer::ChatServer(asio::io_service& io_service, const asio::ip::tcp::endpoint& endpoint) :
	Acceptor(io_service, endpoint),
	TcpSocket(io_service)
{
	DoAccept();
}

void ChatServer::DoAccept()
{
	auto AcceptFunc = [this](std::error_code ec)
	{
		if (!ec)
		{
			std::make_shared<ChatSession>(std::move(TcpSocket), Room)->start();
		}

		DoAccept();
	};

	Acceptor.async_accept(TcpSocket, AcceptFunc);
}
