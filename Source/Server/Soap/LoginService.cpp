#include "LoginService.h"

LoginService& LoginService::Instance()
{
	static LoginService Instance;
	return Instance;
}
