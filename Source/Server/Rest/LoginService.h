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

	struct ResponseCodePlugin
	{
		static char const* const PluginId;
		static int32 Init(soap* s, soap_plugin*, void*);
		static void Destroy(soap* s, soap_plugin* p);
		static int32 ChangeResponse(soap* s, int32 originalResponse, size_t contentLength);

		int32(*fresponse)(soap* s, int32 status, size_t length);
		int32 ErrorCode;
	};
public:
	bool Start(asio::io_service& ioService);
	void Run();
	void Stop();

	int32 SendResponse(soap* soapClient, google::protobuf::Message const& response);
	int32 HandleGet(soap* soapClient);
	int32 HandlePost(soap* soapClient);

	/*

	*/

	void CleanupLoginTickets(const asio::error_code& error);

protected:

};

