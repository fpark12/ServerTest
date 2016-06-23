// Include Header Files
#include <iostream>
#include <cstdio>
#include <cstdlib>

// For MySQL Connection
//#include <mysql.h>

// capn proto
#include "Time.capnp.h"
#include <capnp/message.h>
#include <capnp/serialize-packed.h>
#include <iostream>

void writeDate(int fd)
{
	::capnp::MallocMessageBuilder message;

	Time::Builder timeBuilder = message.initRoot<Time>();
	timeBuilder.setDay(1);
	timeBuilder.setMonth(1);
	timeBuilder.setYear(2016);

	writePackedMessageToFd(fd, message);
}

void printDate(int fd)
{
	::capnp::PackedFdMessageReader message(fd);
	Time::Reader timeReader = message.getRoot<Time>();
	std::cout << "Day: " << timeReader.getDay() << std::endl;
}

int main()
{

}