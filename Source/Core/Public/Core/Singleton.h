#pragma once

class Singleton
{
private:
	Singleton() = default;
	virtual ~Singleton() = default;
	Singleton(Singleton const&) = delete;
	Singleton& operator=(Singleton const&) = delete;

public:
	static Singleton& Instance();

};
//#define GLoginService Singleton::Instance()
//static_assert(std::is_pod<LoginService>::value, "LoginService is not POD!");
