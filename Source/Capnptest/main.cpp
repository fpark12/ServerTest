// Include Header Files
#include <iostream>
#include <cstdio>
#include <cstdlib>

// For MySQL Connection
//#include <mysql.h>

// Prevents Windows headers from defining macros called min and max, which
// conflict with identifiers in the C++ standard library.
#ifndef NOMINMAX
#define NOMINMAX
#endif

// Distinguishes between different types of handles so that we get better
// error checking at compile time.
#ifndef STRICT
#define STRICT
#endif

#include <windows.h>

// capn proto
#include "MyTime.capnp.h"
#include <capnp/capnp.message.h>
#include <capnp/capnp.serialize-packed.h>
#include <iostream>

//#include <mysql.h>

void writeDate(int fd)
{
	::capnp::MallocMessageBuilder message;

	MyTime::Builder timeBuilder = message.initRoot<MyTime>();
	timeBuilder.setDay(1);
	timeBuilder.setMonth(1);
	timeBuilder.setYear(2016);

	writePackedMessageToFd(fd, message);
}

void printDate(int fd)
{
	::capnp::PackedFdMessageReader message(fd);
	MyTime::Reader timeReader = message.getRoot<MyTime>();
	std::cout << "Day: " << timeReader.getDay() << std::endl;
}

int main()
{
	return 0;
}