#pragma once
#include "asio.hpp"

#include <chrono>

#include "httpget.h"
#include "httppost.h"
#include "soapH.h"

#include "Login.pb.h"

int ns1__executeCommand(soap*, char*, char**);

class LoginService
{
	DECLARE_SINGLETON(LoginService);
private:
	std::thread Thread;
	std::string IPAddress;
	std::string BindIP;
	int32 Port;
	asio::ip::tcp::endpoint ExternalAddress;
	asio::ip::tcp::endpoint LocalAddress;
	asio::steady_timer* LoginTicketCleanupTimer;
	bool Stopped;

	Battlenet::JSON::Login::FormInputs LoginFormInputs;
public:
	bool Start(asio::io_service& ioService);

	void Run();

	void Stop();

	int32 SendResponse(soap* soapClient, google::protobuf::Message const& response);

	//friend int32 handle_get_plugin(soap* soapClient);
	int32 HandleGet(soap* soapClient);

	//friend int32 handle_post_plugin(soap* soapClient);
	int32 HandlePost(soap* soapClient);

	/*

	*/

	void CleanupLoginTickets(const asio::error_code& error);

protected:

};

