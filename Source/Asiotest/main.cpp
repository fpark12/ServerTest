// Include Header Files
#include <iostream>
#include "asio.hpp"

int main()
{
	// Your program will have at least one io_service object. The io_service represents your program's link to the operating system's I/O services.
	asio::io_service io_service;

	// To perform I/O operations your program will need an I/O object such as a TCP socket:
	asio::ip::tcp::socket socket(io_service);

	// When a synchronous connect operation is performed, the following sequence of events occurs :
	// 1. Your program initiates the connect operation by calling the I / O object :
	socket.connect(server_endpoint);














	return 0;
}