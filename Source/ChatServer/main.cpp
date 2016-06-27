#include "ChatServer.h"

int main(int argc, char* argv[])
{
	try
	{
		// 		if (argc < 2)
		// 		{
		// 			std::cerr << "Usage: chat_server <port> [<port> ...]\n";
		// 			return 1;
		// 		}

		asio::io_service IoService;

		std::list<ChatServer> Servers;
		for (int i = 0; i < 1; ++i)
		{
			asio::ip::tcp::endpoint Endpoint(asio::ip::tcp::v4(), std::atoi("14493"/*argv[i]*/));
			Servers.emplace_back(IoService, Endpoint);
		}

		IoService.run();
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}