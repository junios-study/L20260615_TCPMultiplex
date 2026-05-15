#define FD_SETSIZE		100

#include <winsock2.h>
#include <iostream>


#pragma comment(lib, "ws2_32")

int main()
{
	WSAData wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET ListenSocket = socket(PF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN ListenSockAddr;
	memset(&ListenSockAddr, 0, sizeof(ListenSockAddr));
	ListenSockAddr.sin_family = AF_INET;
	ListenSockAddr.sin_addr.s_addr = INADDR_ANY;
	ListenSockAddr.sin_port = htons(43000);

	bind(ListenSocket, (SOCKADDR*)&ListenSockAddr, sizeof(ListenSockAddr));

	listen(ListenSocket, 5);

	SOCKADDR_IN ClienSockAddr;
	memset(&ClienSockAddr, 0, sizeof(ClienSockAddr));
	int ClienSockAddrLenght = sizeof(ClienSockAddr);
	
	int ReadSocketCount = 0; //windows X

	fd_set ReadSockets;
	fd_set CopyReadSockets;


	FD_ZERO(&ReadSockets);
	FD_SET(ListenSocket, &ReadSockets);

	struct timeval TimeOut;
	TimeOut.tv_sec = 0;
	TimeOut.tv_usec = 500000;

	int Count = 0;
	while (true)
	{
		CopyReadSockets = ReadSockets;

		//polling
		int ChangeSocketCount = select(0, &CopyReadSockets, 0, 0, &TimeOut);
		if (ChangeSocketCount <= 0)
		{
			//실 서버 작업을 하는거임
			//tick
			//printf("Nothing %d\n", ++Count);
			continue;
		}

		for (int i = 0; i < (int)ReadSockets.fd_count; ++i)
		{
			//바꼈냐?
			if (FD_ISSET(ReadSockets.fd_array[i], &CopyReadSockets))
			{
				//그거 리슨이냐?
				if (ReadSockets.fd_array[i] == ListenSocket)
				{
					SOCKET ClientSocket = accept(ListenSocket, (SOCKADDR*)&ClienSockAddr, &ClienSockAddrLenght);
					printf("connect client.\n");
					FD_SET(ClientSocket, &ReadSockets);
				}
				else
				{
					char Buffer[1024] = { 0, };

					int RecvBytes = recv(ReadSockets.fd_array[i], Buffer, sizeof(Buffer), 0);
					if (RecvBytes <= 0)
					{
						SOCKET ClosedSocket = ReadSockets.fd_array[i];
						printf("disconnect client.\n");
						FD_CLR(ReadSockets.fd_array[i], &ReadSockets);
						closesocket(ClosedSocket);
					}
					else
					{
						//PacketType
						printf("client send : %s\n", Buffer);

						for (int j = 0; j < (int)ReadSockets.fd_count; ++j)
						{
							if (ReadSockets.fd_array[i] != ListenSocket)
							{
								int SentBytes = send(ReadSockets.fd_array[i], Buffer, (int)strlen(Buffer), 0);
								if (SentBytes <= 0)
								{
									SOCKET ClosedSocket = ReadSockets.fd_array[i];
									printf("disconnect client.\n");
									FD_CLR(ReadSockets.fd_array[i], &ReadSockets);
									closesocket(ClosedSocket);
								}
							}
						}
					}
				}
			}
		}
	}


	closesocket(ListenSocket);


	WSACleanup();

	return 0;
}