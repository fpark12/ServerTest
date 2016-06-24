
#include "asio.hpp"

#include "ChatRoom.h"

class ChatServer
{
public:
	ChatServer(asio::io_service& io_service, const asio::ip::tcp::endpoint& endpoint);

private:
	void DoAccept();

	asio::ip::tcp::acceptor Acceptor;
	asio::ip::tcp::socket TcpSocket;
	ChatRoom Room;
};
