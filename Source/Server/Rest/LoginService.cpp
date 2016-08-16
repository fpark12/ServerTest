#include "LoginService.h"

::LoginService& LoginServiceRef = LoginService::Instance();

Ssl& SslRef = Ssl::Instance();

int ns1__executeCommand(soap*, char*, char**)
{
	return SOAP_OK;
}

Namespace namespaces[] =
{
	{ NULL, NULL, NULL, NULL }
};


	LoginService& LoginService::Instance()
	{
		static LoginService Instance;
		return Instance;
	}

