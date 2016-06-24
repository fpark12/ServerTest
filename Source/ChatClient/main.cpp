#include "ChatClient.h"

int main(int argc, char* argv[])
{
	try
	{
		// 		if (argc != 3)
		// 		{
		// 			std::cerr << "Usage: chat_client <host> <port>\n";
		// 			return 1;
		// 		}

		asio::io_service io_service;

		asio::ip::tcp::resolver resolver(io_service);
		auto endpoint_iterator = resolver.resolve({ "73.222.88.181"/*argv[1]*/, "14493" /*argv[2]*/ });
		chat_client c(io_service, endpoint_iterator);

		std::thread t([&io_service]()
		{
			io_service.run();
		});

		char line[ChatMessage::max_body_length + 1];
		while (std::cin.getline(line, ChatMessage::max_body_length + 1))
		{
			ChatMessage msg;
			msg.body_length(std::strlen(line));
			std::memcpy(msg.body(), line, msg.body_length());
			msg.encode_header();
			c.write(msg);
		}

		c.close();
		t.join();
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}