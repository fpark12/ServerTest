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

	Battlenet::JSON::Login::FormInputs _formInputs;
public:
	bool Start(asio::io_service& ioService);

	void Run();

	void Stop();

	//friend int32 handle_get_plugin(soap* soapClient);
	int32 HandleGet(soap* soapClient);

	//friend int32 handle_post_plugin(soap* soapClient);
	int32 HandlePost(soap* soapClient);

	/*
	int32 LoginRESTService::SendResponse(soap* soapClient, google::protobuf::Message const& response)
	{
		std::string jsonResponse = JSON::Serialize(response);

		soap_response(soapClient, SOAP_FILE);
		soap_send_raw(soapClient, jsonResponse.c_str(), jsonResponse.length());
		return soap_end_send(soapClient);
	}
	*/

	void CleanupLoginTickets(const asio::error_code& error);

protected:

};

