#include "SslContext.h"
#include "LoginService.h"
#include "Server.h"

int ns1__executeCommand(soap*, char*, char**)
{
	return SOAP_OK;
}

Namespace namespaces[] =
{
	{ NULL, NULL, NULL, NULL }
};

bool LoginService::Start(asio::io_service& ioService)
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

	// set up form inputs
	Battlenet::JSON::Login::FormInput* input;
	LoginFormInputs.set_type(Battlenet::JSON::Login::LOGIN_FORM);
	input = LoginFormInputs.add_inputs();
	input->set_input_id("account_name");
	input->set_type("text");
	input->set_label("E-mail");
	input->set_max_length(320);

	input = LoginFormInputs.add_inputs();
	input->set_input_id("password");
	input->set_type("password");
	input->set_label("Password");
	input->set_max_length(16);

	input = LoginFormInputs.add_inputs();
	input->set_input_id("log_in_submit");
	input->set_type("submit");
	input->set_label("Log In");

	LoginTicketCleanupTimer = new asio::steady_timer(ioService);
	LoginTicketCleanupTimer->expires_from_now(std::chrono::seconds(3));
	LoginTicketCleanupTimer->async_wait(std::bind(&LoginService::CleanupLoginTickets, this, std::placeholders::_1));

	Thread = std::thread(std::bind(&LoginService::Run, this));

	ioService.run();
	while (true)
	{

	}
	/*










	*/

	return true;
}

int32 handle_get_plugin(soap* soapClient)
{
	return GLoginService.HandleGet(soapClient);
}

int32 handle_post_plugin(soap* soapClient)
{
	return GLoginService.HandlePost(soapClient);
}

#ifdef _WIN32
#include <windows.h>
const DWORD MS_VC_EXCEPTION = 0x406D1388;

#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO
{
	DWORD dwType; // Must be 0x1000.
	LPCSTR szName; // Pointer to name (in user addr space).
	DWORD dwThreadID; // Thread ID (-1=caller thread).
	DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)
void SetThreadName(uint32_t dwThreadID, const char* threadName)
{

	// DWORD dwThreadID = ::GetThreadId( static_cast<HANDLE>( t.native_handle() ) );

	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = threadName;
	info.dwThreadID = dwThreadID;
	info.dwFlags = 0;

	__try
	{
		RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}
}
#endif

void LoginService::Run()
{
	soap SoapInstance(SOAP_C_UTFSTRING, SOAP_C_UTFSTRING);

	// check every 3 seconds if world ended
	SoapInstance.accept_timeout = 3;
	SoapInstance.recv_timeout = 5;
	SoapInstance.send_timeout = 5;
	if (!soap_valid_socket(soap_bind(&SoapInstance, BindIP.c_str(), Port, 100)))
	{
		GConsole.Message("Couldn't bind to {0}:{1}", BindIP.c_str(), Port);
		return;
	}

	GConsole.Message("Login service bound to http://{0}:{1}", BindIP.c_str(), Port);
	//TC_LOG_INFO("server.rest", "Login service bound to http://%s:%d", _bindIP.c_str(), _port);

	//auto handle_get_plugin2 = [](soap* soapClient)
	//{
	//	return GLoginService.HandleGet(soapClient);
	//};

	//auto handle_post_plugin2 = [](soap* soapClient)
	//{
	//	return GLoginService.HandlePost(soapClient);
	//};

	http_post_handlers handlers[] =
	{
		{ "application/json;charset=utf-8", handle_post_plugin },
		{ "application/json", handle_post_plugin },
		{ nullptr, nullptr }
	};

	soap_register_plugin_arg(&SoapInstance, &http_get, (void*)&handle_get_plugin/*std::bind(&LoginService::HandleGet, &GLoginService)*/);
	soap_register_plugin_arg(&SoapInstance, &http_post, handlers);
	//soap_register_plugin_arg(&soapServer, &ContentTypePlugin::Init, (void*)"application/json;charset=utf-8");

	// Use our already ready ssl context
	//GSslContext.Initialize();
	//SoapInstance.ctx = GSslContext.Context.native_handle();
	//sSoapInstance.ssl_flags = SOAP_SSL_NO_AUTHENTICATION;// SOAP_SSL_RSA;

	while (!Stopped)
	{
		if (!soap_valid_socket(soap_accept(&SoapInstance)))
			continue;   // ran into an accept timeout

		//if (soap_ssl_accept(&SoapInstance) != SOAP_OK)
		//{
			//TC_LOG_DEBUG("server.rest", "Failed SSL handshake from IP=%s", IPAddress.to_string().c_str());
		//	continue;
		//}

		//TC_LOG_DEBUG("server.rest", "Accepted connection from IP=%s", IPAddress.to_string().c_str());
		
		std::shared_ptr<soap> SoapInstanceCopy = std::make_shared<soap>(SoapInstance);
		asio::ip::address_v4 IPAddress(SoapInstanceCopy->ip);
		GConsole.Message("Accepted connetion from IP={0}.\n", IPAddress.to_string().c_str());

		auto SoapMain = [SoapInstanceCopy]
		{
			soap_serve(SoapInstanceCopy.get());
			//GConsole.Message("Thread Ended.\n");
		};

		std::thread Thread = std::thread(SoapMain);
		DWORD threadId = ::GetThreadId(static_cast<HANDLE>(Thread.native_handle()));
		SetThreadName(threadId, "SoapThread");
		Thread.detach();

	}

	// and release the context handle here - soap does not own it so it should not free it on exit
	SoapInstance.ctx = nullptr;

	//TC_LOG_INFO("server.rest", "Login service exiting...");

	/*






	*/
}

void LoginService::Stop()
{

}

int32 LoginService::SendResponse(soap* soapClient, google::protobuf::Message const& response)
{
	std::string jsonResponse = JSON::Serialize(response);

	soap_response(soapClient, SOAP_FILE);
	soap_send_raw(soapClient, jsonResponse.c_str(), jsonResponse.length());
	return soap_end_send(soapClient);
}

int32 LoginService::HandleGet(soap* soapClient)
{
	asio::ip::address_v4 address(soapClient->ip);
	std::string ip_address = address.to_string();

	GConsole.Message("[{0}:{1}] Handling GET request path=\"{2}\"", ip_address.c_str(), soapClient->port, soapClient->path);

	//TC_LOG_DEBUG("server.rest", "[%s:%d] Handling GET request path=\"%s\"", ip_address.c_str(), soapClient->port, soapClient->path);

	static std::string const expectedPath = "/bnetserver/login/";
	if (strstr(soapClient->path, expectedPath.c_str()) != &soapClient->path[0])
		return 404;

	SendResponse(soapClient, LoginFormInputs);

	return 0;
}

int32 LoginService::HandlePost(soap* soapClient)
{
	return 0;
}

void LoginService::CleanupLoginTickets(const asio::error_code& error)
{
	printf("Cleaning up...\n");
	LoginTicketCleanupTimer->expires_from_now(std::chrono::seconds(10));
	LoginTicketCleanupTimer->async_wait(std::bind(&LoginService::CleanupLoginTickets, this, std::placeholders::_1));
}
