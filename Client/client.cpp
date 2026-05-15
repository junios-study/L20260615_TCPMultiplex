#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define FD_SETSIZE		100

#include <winsock2.h>
#include <iostream>


#pragma comment(lib, "ws2_32")

int main()
{
	WSAData wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET ServerSocket = socket(PF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN ServerSockAddr;
	memset(&ServerSockAddr, 0, sizeof(ServerSockAddr));
	ServerSockAddr.sin_family = AF_INET;
	ServerSockAddr.sin_addr.s_addr = inet_addr("192.168.0.95");
	ServerSockAddr.sin_port = htons(43000);

	connect(ServerSocket, (SOCKADDR*)&ServerSockAddr, sizeof(ServerSockAddr));

	while (true)
	{
		char Message[1024] = { 0, };
		std::cin.getline(Message, '\n');

		int SentBytes = send(ServerSocket, Message, (int)strlen(Message), 0);

		if (SentBytes <= 0)
		{
			break;
		}
	}

	closesocket(ServerSocket);


	WSACleanup();

	return 0;
}