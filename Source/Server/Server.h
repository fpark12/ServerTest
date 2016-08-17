#pragma once
#include "LoginService.h"

static LoginService& GLoginServiceInst = Singleton<LoginService>().Instance();
#define GLoginService GLoginServiceInst
