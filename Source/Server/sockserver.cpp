// sockserver.cpp : Defines the entry point for the console application.
//

#define WIN32_LEAN_AND_MEAN
//#include "stdafx.h"
#include <winsock2.h>
#include <process.h>
#pragma comment(lib,"ws2_32.lib" )

unsigned int __stdcall  ServClient(void *data);
/*
int _tmain(int argc, _TCHAR* argv[])
{

	WSADATA wsaData;
	int iResult;
	sockaddr_in addr;
	SOCKET sock,client;

	addr.sin_family = AF_INET;
	addr.sin_port = htons(2222);
	addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	iResult = WSAStartup(MAKEWORD(2,2),&wsaData);//2.2 

	if(iResult)
	{
		printf("WSA startup failed");
		return 0;
	}


	sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	
	if(sock == INVALID_SOCKET)
	{
		printf("Invalid socket");
		return 0;
	}

	iResult = bind(sock,(sockaddr*)&addr,sizeof(sockaddr_in ));
	
	if(iResult)
	{
		
		printf("bind failed %u",GetLastError());
		
		return 0;
	}

	iResult = listen(sock,SOMAXCONN);
	
	if(iResult)
	{
		
		printf("iResult failed %u",GetLastError());
		
		return 0;
	}

	while(client = accept(sock,0,0))
	{
		if(client == INVALID_SOCKET)
		{
			printf("invalid client socket",GetLastError());
			continue;
		}	
		_beginthreadex(0,0,ServClient,(void*)&client,0,0);
		
	}


	return 0;
}
*/
unsigned int __stdcall ServClient(void *data)
{
	SOCKET* client =(SOCKET*)data;
    SOCKET Client = *client;
	//printf("Client connected\n");
	
	char chunk[200];
	while(recv(Client,chunk,200,0))
	{
		//printf("%s \t %d\n",chunk,GetCurrentThreadId());
	}
return 0;
}