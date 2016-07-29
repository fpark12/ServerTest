#include "LoginService.h"

LoginService& LoginServiceRef = LoginService::Instance();
LoginService& LoginService::Instance()
{
	static LoginService Instance;
	return Instance;
}
