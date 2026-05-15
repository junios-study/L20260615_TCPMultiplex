#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define FD_SETSIZE		100



#include <winsock2.h>
#include <Windows.h>

#include <iostream>
#include <process.h>

#include <atomic>

#pragma comment(lib, "ws2_32")

//unsigned 경준이돈 = 10000;
std::atomic<long> 경준이돈 = 10000;

unsigned Increasement(void* Argument)
{
	for (int i = 0; i < 10000; ++i)
	{

		//문이 열려있으면 문닫고
		//원자적으로 작업(원자적으로 더하기)
		//InterlockedIncrement(&경준이돈); //windows 전용
		경준이돈.fetch_add(1);

		//경준이돈++;
		//1. 글로벌 변수 접근(숫자 가져오기)
		//2. 가져온 숫자에 +1
		//3. 다시 글로벌 변수에 저장
		//문열고
	}
	return 0;
}

unsigned Decreasement(void* Argument)
{
	for (int i = 0; i < 10000; ++i)
	{
		//문이 열려있으면 문닫고
		//원자적
		//InterlockedDecrement(&경준이돈);
		경준이돈.fetch_sub(1);

		//경준이돈--;
		//1. 글로벌 변수 접근(숫자 가져오기)
		//2. 가져온 숫자에 -1
		//3. 다시 글로벌 변수에 저장
		//문열고

	}
	return 0;
}


//race condition

int main()
{
	HANDLE ThreadHanle1 = (HANDLE)_beginthreadex(0, 0, &Increasement, 0, 0, 0);
	HANDLE ThreadHanle2 = (HANDLE)_beginthreadex(0, 0, &Decreasement, 0, 0, 0);
	HANDLE ThreadHanle3 = (HANDLE)_beginthreadex(0, 0, &Increasement, 0, 0, 0);
	HANDLE ThreadHanle4 = (HANDLE)_beginthreadex(0, 0, &Decreasement, 0, 0, 0);

	Sleep(2000);

	std::cout << "경준이 돈은?" << 경준이돈 << std::endl;

	return 0;
}


int main2()
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
		std::cin.getline(Message, 1024);

		int SentBytes = send(ServerSocket, Message, (int)strlen(Message), 0);

		if (SentBytes <= 0)
		{
			break;
		}

		int RecvBytes = recv(ServerSocket, Message, (int)sizeof(Message), 0);
		if (RecvBytes <= 0)
		{
			break;
		}

		printf("server send : %s\n", Message);
	}

	closesocket(ServerSocket);


	WSACleanup();

	return 0;
}