#include <iostream>
#include <string>
#include <thread>
#include <mutex>

#include "class.h"

void Functor::operator()(std::string& msg)
{
	msg = "trust is the mother of deceit";
	for(int i = 0; i < 3; i++)
	{
		std::cout << "From Functor: " << msg << std::endl;
	}
}

void Functor::operator()()
{
	for(int i = 0; i < 100; i++)
	{
		std::cout << "From Functor: " << i << std::endl;
	}
}

