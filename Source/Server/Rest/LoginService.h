#pragma once
#include "asio.hpp"
#include <asio/ssl/context.hpp>

#include "Public/Config/config.h"
#include <chrono>

#include "httpget.h"
#include "httppost.h"
#include "soapH.h"

class Ssl;
class LoginService;

extern Ssl& SslRef;
extern LoginService& LoginServiceRef;
#define GSsl SslRef
#define GLoginService LoginServiceRef

class Ssl
{
private:
	Ssl() = default;
	~Ssl() = default;

public:
	void Initialize()
	{
		asio::error_code err;
		bool Res = 0;

		std::string CertificateChainFile;
		std::string PrivateKeyFile;
		Res |= GConfig.GetString("LoginService.Initialization.CertificatesFile", CertificateChainFile);
		Res |= GConfig.GetString("LoginService.Initialization.PrivateKeyFile", PrivateKeyFile);


		Context.set_options(asio::ssl::context::no_sslv3, err);
		assert(err.value() == 0);
		Context.use_certificate_chain_file(CertificateChainFile, err);
		assert(err.value() == 0);
		Context.use_private_key_file(PrivateKeyFile, asio::ssl::context::pem, err);
		assert(err.value() == 0);
	}

	static Ssl& Instance()
	{
		static Ssl Instance;
		return Instance;
	}

	asio::ssl::context Context = asio::ssl::context(asio::ssl::context::sslv23);

};

int ns1__executeCommand(soap*, char*, char**);

class LoginService
{
private:
	LoginService() = default;
	~LoginService() = default;
	LoginService(LoginService const&) = delete;
	LoginService& operator=(LoginService const&) = delete;

	std::thread Thread;
	std::string IPAddress;
	std::string BindIP;
	int32 Port;
	asio::ip::tcp::endpoint ExternalAddress;
	asio::ip::tcp::endpoint LocalAddress;
	asio::steady_timer* LoginTicketCleanupTimer;
	bool Stopped;
public:
	static LoginService& Instance();

	bool Start(asio::io_service& ioService)
	{
		bool Res = 0;
		asio::error_code ec;
		std::string TempExternalAddress;
		std::string TempLocalAddress;
		asio::ip::tcp::resolver::iterator end;
		asio::ip::tcp::resolver resolver(ioService);

		// "LoginService.Init.IPAddress	"
		BindIP = "0.0.0.0";
		Res |= GConfig.GetString("LoginService.Initialization.IPAddress", IPAddress);
		Res |= GConfig.GetString("LoginService.Initialization.ExternalIPAddress", TempExternalAddress);
		Res |= GConfig.GetString("LoginService.Initialization.LocalIPAddress", TempLocalAddress);
		Res |= GConfig.GetInt("LoginService.Initialization.Port", Port);
		if (Port < 0 || Port > 0xFFFF)
		{
			//TC_LOG_ERROR("server.rest", "Specified login service port (%d) out of allowed range (1-65535), defaulting to 8081", _port);
			Port = 8081;
		}

		asio::ip::tcp::resolver::query externalAddressQuery(asio::ip::tcp::v4(), TempExternalAddress, std::to_string(Port));
		asio::ip::tcp::resolver::iterator endPoint = resolver.resolve(externalAddressQuery, ec);

		if (endPoint == end || ec)
		{
			//TC_LOG_ERROR("server.rest", "Could not resolve LoginREST.ExternalAddress %s", configuredAddress.c_str());
			return false;
		}

		ExternalAddress = endPoint->endpoint();

		asio::ip::tcp::resolver::query localAddressQuery(asio::ip::tcp::v4(), TempLocalAddress, std::to_string(Port));
		endPoint = resolver.resolve(localAddressQuery, ec);
		if (endPoint == end || ec)
		{
			//TC_LOG_ERROR("server.rest", "Could not resolve LoginREST.ExternalAddress %s", TempLocalAddress.c_str());
			return false;
		}
		LocalAddress = endPoint->endpoint();

		LoginTicketCleanupTimer = new asio::steady_timer(ioService);
		LoginTicketCleanupTimer->expires_from_now(std::chrono::seconds(3));
		LoginTicketCleanupTimer->async_wait(std::bind(&LoginService::CleanupLoginTickets, this, std::placeholders::_1));

		Thread = std::thread(std::bind(&LoginService::Run, this));

		ioService.run();
		while (true)
		{

		}
		/*







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


		*/

		return true;
	}

	void Run()
	{
		soap SoapInstance(SOAP_C_UTFSTRING, SOAP_C_UTFSTRING);

		// check every 3 seconds if world ended
		SoapInstance.accept_timeout = 3;
		SoapInstance.recv_timeout = 5;
		SoapInstance.send_timeout = 5;
		if (!soap_valid_socket(soap_bind(&SoapInstance, BindIP.c_str(), Port, 100)))
		{
			//TC_LOG_ERROR("server.rest", "Couldn't bind to %s:%d", _bindIP.c_str(), _port);
			return;
		}

		//TC_LOG_INFO("server.rest", "Login service bound to http://%s:%d", _bindIP.c_str(), _port);

		auto handle_get_plugin = [](soap* soapClient)
		{
			return GLoginService.HandleGet(soapClient);
		};

		auto handle_post_plugin = [](soap* soapClient)
		{
			return GLoginService.HandlePost(soapClient);
		};

		http_post_handlers handlers[] =
		{
			{ "application/json;charset=utf-8", handle_post_plugin },
			{ "application/json", handle_post_plugin },
			{ nullptr, nullptr }
		};

		soap_register_plugin_arg(&SoapInstance, &http_get, (void*)&handle_get_plugin);
		soap_register_plugin_arg(&SoapInstance, &http_post, handlers);
		//soap_register_plugin_arg(&soapServer, &ContentTypePlugin::Init, (void*)"application/json;charset=utf-8");

		// Use our already ready ssl context
		GSsl.Initialize();
		SoapInstance.ctx = GSsl.Context.native_handle();
		SoapInstance.ssl_flags = SOAP_SSL_RSA;

		while (!Stopped)
		{
			if (!soap_valid_socket(soap_accept(&SoapInstance)))
				continue;   // ran into an accept timeout

			asio::ip::address_v4 address(SoapInstance.ip);
			if (soap_ssl_accept(&SoapInstance) != SOAP_OK)
			{
				//TC_LOG_DEBUG("server.rest", "Failed SSL handshake from IP=%s", address.to_string().c_str());
				continue;
			}

			//TC_LOG_DEBUG("server.rest", "Accepted connection from IP=%s", address.to_string().c_str());

			auto SoapMain = [&SoapInstance]
			{
				soap_serve(&SoapInstance);
			};

			std::thread(SoapMain).detach();
		}

		// and release the context handle here - soap does not own it so it should not free it on exit
		SoapInstance.ctx = nullptr;

		//TC_LOG_INFO("server.rest", "Login service exiting...");

		/*






		*/
	}

	void Stop()
	{

	}

	//friend int32 handle_get_plugin(soap* soapClient);
	int32 HandleGet(soap* soapClient)
	{
		asio::ip::address_v4 address(soapClient->ip);
		std::string ip_address = address.to_string();

		//TC_LOG_DEBUG("server.rest", "[%s:%d] Handling GET request path=\"%s\"", ip_address.c_str(), soapClient->port, soapClient->path);

		static std::string const expectedPath = "/bnetserver/login/";
		if (strstr(soapClient->path, expectedPath.c_str()) != &soapClient->path[0])
			return 404;

		return 0;// SendResponse(soapClient, _formInputs);
	}

	//friend int32 handle_post_plugin(soap* soapClient);
	int32 HandlePost(soap* soapClient)
	{
		return 0;
	}

	/*
	int32 LoginRESTService::SendResponse(soap* soapClient, google::protobuf::Message const& response)
	{
		std::string jsonResponse = JSON::Serialize(response);

		soap_response(soapClient, SOAP_FILE);
		soap_send_raw(soapClient, jsonResponse.c_str(), jsonResponse.length());
		return soap_end_send(soapClient);
	}
	*/


	void CleanupLoginTickets(const asio::error_code& error)
	{
		printf("Cleaning up...\n");
		LoginTicketCleanupTimer->expires_from_now(std::chrono::seconds(3));
		LoginTicketCleanupTimer->async_wait(std::bind(&LoginService::CleanupLoginTickets, this, std::placeholders::_1));

	}


protected:

};

//static_assert(std::is_pod<LoginService>::value, "LoginService is not POD!");

