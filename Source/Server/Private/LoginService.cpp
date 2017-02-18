#include "SslContext.h"
#include "LoginService.h"
#include "Server.h"

#include "Public/Detail/Cryptography.h"

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
	input->set_inputid("account_name");
	input->set_type("text");
	input->set_label("E-mail");
	input->set_maxlength(320);

	input = LoginFormInputs.add_inputs();
	input->set_inputid("password");
	input->set_type("password");
	input->set_label("Password");
	input->set_maxlength(16);

	input = LoginFormInputs.add_inputs();
	input->set_inputid("log_in_submit");
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

	int32(*handle_get_plugin)(soap*) = [](soap* soapClient)
	{
		return GLoginService.HandleGet(soapClient);
	};

	int32(*handle_post_plugin)(soap*) = [](soap* soapClient)
	{
		return GLoginService.HandlePost(soapClient);
	};

	http_post_handlers handlers[] =
	{
		{ "application/json;charset=utf-8", handle_post_plugin },
		{ "application/json", handle_post_plugin },
		{ nullptr, nullptr }
	};

	soap_register_plugin_arg(&SoapInstance, &http_get, (void*)handle_get_plugin);
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

}

void LoginService::Stop()
{

}

int32 LoginService::SendResponse(soap* soapClient, google::protobuf::Message const& response)
{
	std::string JsonResponse;
	Protobuf::MessageToJson(response, JsonResponse);
	//std::string jsonResponse = JSON::Serialize(response);
	//int result = JsonResponse.compare(jsonResponse.c_str());

	soap_response(soapClient, SOAP_FILE);
	soap_send_raw(soapClient, JsonResponse.c_str(), JsonResponse.length());
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
	{
		return 404;
	}

	SendResponse(soapClient, LoginFormInputs);

	return 0;
}

int32 LoginService::HandlePost(soap* soapClient)
{

	asio::ip::address_v4 address(soapClient->ip);
	std::string ip_address = address.to_string();

	GConsole.Message("[{0}:{1}] Handling POST request path=\"{2}\"", ip_address.c_str(), soapClient->port, soapClient->path);

	static std::string const expectedPath = "/bnetserver/login/";
	if (strstr(soapClient->path, expectedPath.c_str()) != &soapClient->path[0])
	{
		return 404;
	}

	char *Buffer;
	size_t Length;
	soap_http_body(soapClient, &Buffer, &Length);

	Battlenet::JSON::Login::LoginForm LoginForm;
	Battlenet::JSON::Login::LoginResult LoginResult;
	//if (!JSON::Deserialize(Buffer, &LoginForm))
	if (Protobuf::JsonToMessage(Buffer, LoginForm))
	{
		if (soap_register_plugin_arg(soapClient, &ResponseCodePlugin::Init, nullptr) != SOAP_OK)
			return 500;

		ResponseCodePlugin* ResponseCode = reinterpret_cast<ResponseCodePlugin*>(soap_lookup_plugin(soapClient, ResponseCodePlugin::PluginId));
		ASSERT(ResponseCode);

		ResponseCode->ErrorCode = 400;

		LoginResult.set_authenticationstate(Battlenet::JSON::Login::LOGIN);
		LoginResult.set_errorcode("UNABLE_TO_DECODE");
		LoginResult.set_errormessage("There was an internal error while connecting to Battle.net. Please try again later.");
		return SendResponse(soapClient, LoginResult);
	}

	std::string Login;
	std::string Password;

	for (int32 i = 0; i < LoginForm.inputs_size(); ++i)
	{
		if (LoginForm.inputs(i).inputid() == "account_name")
		{
			Login = LoginForm.inputs(i).value();
		}
		else if (LoginForm.inputs(i).inputid() == "password")
		{
			Password = LoginForm.inputs(i).value();
		}
	}

	Login = Utf8ToUpperLatin(Login);
	Password = Utf8ToUpperLatin(Password);

	/*
	PreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_BNET_ACCOUNT_INFO);
	stmt->setString(0, login);

	if (PreparedQueryResult result = LoginDatabase.Query(stmt))
	{
		std::string pass_hash = result->Fetch()[13].GetString();

		std::unique_ptr<Battlenet::Session::AccountInfo> accountInfo = Trinity::make_unique<Battlenet::Session::AccountInfo>();
		accountInfo->LoadResult(result);

		if (CalculateShaPassHash(login, std::move(password)) == pass_hash)
		{
			stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_BNET_CHARACTER_COUNTS_BY_BNET_ID);
			stmt->setUInt32(0, accountInfo->Id);
			if (PreparedQueryResult characterCountsResult = LoginDatabase.Query(stmt))
			{
				do
				{
					Field* fields = characterCountsResult->Fetch();
					accountInfo->GameAccounts[fields[0].GetUInt32()]
						.CharacterCounts[Battlenet::RealmHandle{ fields[3].GetUInt8(), fields[4].GetUInt8(), fields[2].GetUInt32() }.GetAddress()] = fields[1].GetUInt8();

				} while (characterCountsResult->NextRow());
			}

			stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_BNET_LAST_PLAYER_CHARACTERS);
			stmt->setUInt32(0, accountInfo->Id);
			if (PreparedQueryResult lastPlayerCharactersResult = LoginDatabase.Query(stmt))
			{
				Field* fields = lastPlayerCharactersResult->Fetch();
				Battlenet::RealmHandle realmId{ fields[1].GetUInt8(), fields[2].GetUInt8(), fields[3].GetUInt32() };
				Battlenet::Session::LastPlayedCharacterInfo& lastPlayedCharacter = accountInfo->GameAccounts[fields[0].GetUInt32()]
					.LastPlayedCharacters[realmId.GetSubRegionAddress()];

				lastPlayedCharacter.RealmId = realmId;
				lastPlayedCharacter.CharacterName = fields[4].GetString();
				lastPlayedCharacter.CharacterGUID = fields[5].GetUInt64();
				lastPlayedCharacter.LastPlayedTime = fields[6].GetUInt32();
			}

			BigNumber ticket;
			ticket.SetRand(20 * 8);

			LoginResult.set_login_ticket("TC-" + ByteArrayToHexStr(ticket.AsByteArray(20).get(), 20));

			AddLoginTicket(LoginResult.login_ticket(), std::move(accountInfo));
		}
		else if (!accountInfo->IsBanned)
		{
			uint32 maxWrongPassword = uint32(sConfigMgr->GetIntDefault("WrongPass.MaxCount", 0));

			if (sConfigMgr->GetBoolDefault("WrongPass.Logging", false))
				TC_LOG_DEBUG("server.rest", "[%s, Account %s, Id %u] Attempted to connect with wrong password!", ip_address.c_str(), login.c_str(), accountInfo->Id);

			if (maxWrongPassword)
			{
				SQLTransaction trans = LoginDatabase.BeginTransaction();
				stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_BNET_FAILED_LOGINS);
				stmt->setUInt32(0, accountInfo->Id);
				trans->Append(stmt);

				++accountInfo->FailedLogins;

				TC_LOG_DEBUG("server.rest", "MaxWrongPass : %u, failed_login : %u", maxWrongPassword, accountInfo->Id);

				if (accountInfo->FailedLogins >= maxWrongPassword)
				{
					BanMode banType = BanMode(sConfigMgr->GetIntDefault("WrongPass.BanType", uint16(BanMode::BAN_IP)));
					int32 banTime = sConfigMgr->GetIntDefault("WrongPass.BanTime", 600);

					if (banType == BanMode::BAN_ACCOUNT)
					{
						stmt = LoginDatabase.GetPreparedStatement(LOGIN_INS_BNET_ACCOUNT_AUTO_BANNED);
						stmt->setUInt32(0, accountInfo->Id);
					}
					else
					{
						stmt = LoginDatabase.GetPreparedStatement(LOGIN_INS_IP_AUTO_BANNED);
						stmt->setString(0, ip_address);
					}

					stmt->setUInt32(1, banTime);
					trans->Append(stmt);

					stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_BNET_RESET_FAILED_LOGINS);
					stmt->setUInt32(0, accountInfo->Id);
					trans->Append(stmt);
				}

				LoginDatabase.CommitTransaction(trans);
			}
		}
	}

	LoginResult.set_authentication_state(Battlenet::JSON::Login::DONE);
	return SendResponse(soapClient, LoginResult);
	*/
	return 0;
}

char const* const LoginService::ResponseCodePlugin::PluginId = "bnet-error-code";

int32 LoginService::ResponseCodePlugin::Init(soap* s, soap_plugin* p, void* arg)
{
	ResponseCodePlugin* data = new ResponseCodePlugin();
	data->fresponse = s->fresponse;

	p->id = PluginId;
	p->fdelete = &Destroy;
	p->data = data;

	s->fresponse = &ChangeResponse;
	return SOAP_OK;
}

void LoginService::ResponseCodePlugin::Destroy(soap* s, soap_plugin* p)
{
	ResponseCodePlugin* data = reinterpret_cast<ResponseCodePlugin*>(p->data);
	s->fresponse = data->fresponse;
	delete data;
}

int32 LoginService::ResponseCodePlugin::ChangeResponse(soap* s, int32 originalResponse, size_t contentLength)
{
	ResponseCodePlugin* self = reinterpret_cast<ResponseCodePlugin*>(soap_lookup_plugin(s, PluginId));
	return self->fresponse(s, self->ErrorCode && (originalResponse == SOAP_FILE) ? self->ErrorCode : originalResponse, contentLength);
}
void LoginService::CleanupLoginTickets(const asio::error_code& error)
{
	printf("Cleaning up...\n");
	LoginTicketCleanupTimer->expires_from_now(std::chrono::seconds(10));
	LoginTicketCleanupTimer->async_wait(std::bind(&LoginService::CleanupLoginTickets, this, std::placeholders::_1));
}

std::string LoginService::CalculateShaPassHash(std::string const& name, std::string const& password)
{
	SHA256Hash email;
	email.AppendData(name);
	email.CalculateHash();

	SHA256Hash sha;
	sha.AppendData(ByteArrayToHexStr(email.GetDigest(), email.GetLength()));
	sha.AppendData(":");
	sha.AppendData(password);
	sha.CalculateHash();

	return ByteArrayToHexStr(sha.GetDigest(), sha.GetLength(), true);
}