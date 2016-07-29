#pragma once
#include "asio.hpp"
#include "Public/config.h"

class LoginService
{
private:
	LoginService() = default;
	~LoginService() = default;
	LoginService(LoginService const&) = delete;
	LoginService& operator=(LoginService const&) = delete;

	std::string IPAddress;
	int32 Port;
	asio::ip::tcp::endpoint ExternalAddress;

public:
	static LoginService& Instance();

	bool Start(asio::io_service& ioService)
	{
		bool ok;
		asio::error_code ec;
		std::string configuredAddress;
		asio::ip::tcp::resolver::iterator end;

		ok &= GConfig.GetString("Initialization", "IPAddress", IPAddress, "LoginService.ini");
		ok &= GConfig.GetString("Initialization", "ExternalIPAddress", configuredAddress, "LoginService.ini");
		ok &= GConfig.GetInt("Initialization", "Port", Port, "LoginService.ini");
		if (Port < 0 || Port > 0xFFFF)
		{
			//TC_LOG_ERROR("server.rest", "Specified login service port (%d) out of allowed range (1-65535), defaulting to 8081", _port);
			Port = 8081;
		}

		asio::ip::tcp::resolver resolver(ioService);

		asio::ip::tcp::resolver::query externalAddressQuery(asio::ip::tcp::v4(), configuredAddress, std::to_string(Port));
		asio::ip::tcp::resolver::iterator endPoint = resolver.resolve(externalAddressQuery, ec);

		if (endPoint == end || ec)
		{
			//TC_LOG_ERROR("server.rest", "Could not resolve LoginREST.ExternalAddress %s", configuredAddress.c_str());
			return false;
		}

		ExternalAddress = endPoint->endpoint();

		/*




		configuredAddress = GConfig->GetStringDefault("LoginREST.LocalAddress", "127.0.0.1");
		boost::asio::ip::tcp::resolver::query localAddressQuery(boost::asio::ip::tcp::v4(), configuredAddress, std::to_string(_port));
		endPoint = resolver.resolve(localAddressQuery, ec);
		if (endPoint == end || ec)
		{
			TC_LOG_ERROR("server.rest", "Could not resolve LoginREST.ExternalAddress %s", configuredAddress.c_str());
			return false;
		}

		_localAddress = endPoint->endpoint();

		// set up form inputs
		Battlenet::JSON::Login::FormInput* input;
		_formInputs.set_type(Battlenet::JSON::Login::LOGIN_FORM);
		input = _formInputs.add_inputs();
		input->set_input_id("account_name");
		input->set_type("text");
		input->set_label("E-mail");
		input->set_max_length(320);

		input = _formInputs.add_inputs();
		input->set_input_id("password");
		input->set_type("password");
		input->set_label("Password");
		input->set_max_length(16);

		input = _formInputs.add_inputs();
		input->set_input_id("log_in_submit");
		input->set_type("submit");
		input->set_label("Log In");

		_loginTicketCleanupTimer = new boost::asio::deadline_timer(ioService);
		_loginTicketCleanupTimer->expires_from_now(boost::posix_time::seconds(10));
		_loginTicketCleanupTimer->async_wait(std::bind(&LoginRESTService::CleanupLoginTickets, this, std::placeholders::_1));

		_thread = std::thread(std::bind(&LoginRESTService::Run, this));
		*/

		return true;
	}

	void Stop()
	{

	}
	
protected:

};
#define GLoginService LoginServiceRef
//static_assert(std::is_pod<LoginService>::value, "LoginService is not POD!");
