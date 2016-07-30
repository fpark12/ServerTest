#include "LoginService.h"

LoginService& LoginServiceRef = LoginService::Instance();

int ns1__executeCommand(soap*, char*, char**)
{
	return SOAP_OK;
}

LoginService& LoginService::Instance()
{
	static LoginService Instance;
	return Instance;
}


Namespace namespaces[] =
{
	{ NULL, NULL, NULL, NULL }
};