#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define FD_SETSIZE		100




#include <winsock2.h>
#include <Windows.h>

#include <iostream>
#include <process.h>

#include <atomic>
#include <thread>

#pragma comment(lib, "winmm")
#pragma comment(lib, "ws2_32")

unsigned 경준이돈 = 10000;
//std::atomic<long> 경준이돈 = 10000;

CRITICAL_SECTION 경준이돈CS1;
CRITICAL_SECTION 경준이돈CS2;


unsigned Increasement(void* Argument)
{

	for (int i = 0; i < 1000000; ++i)
	{

		//문이 열려있으면 문닫고
		//원자적으로 작업(원자적으로 더하기)
		//InterlockedIncrement(&경준이돈); //windows 전용
		//경준이돈.fetch_add(1);
		EnterCriticalSection(&경준이돈CS1);
		EnterCriticalSection(&경준이돈CS2);
		경준이돈++;
		LeaveCriticalSection(&경준이돈CS2);
		LeaveCriticalSection(&경준이돈CS1);
		//1. 글로벌 변수 접근(숫자 가져오기)
		//2. 가져온 숫자에 +1
		//3. 다시 글로벌 변수에 저장
		//문열고
	}


	return 0;
}

unsigned Decreasement(void* Argument)
{


	for (int i = 0; i < 1000000; ++i)
	{
		//문이 열려있으면 문닫고
		//원자적
		//InterlockedDecrement(&경준이돈);
		//경준이돈.fetch_sub(1);
		EnterCriticalSection(&경준이돈CS1);
		EnterCriticalSection(&경준이돈CS2);
		경준이돈--;
		LeaveCriticalSection(&경준이돈CS1);
		LeaveCriticalSection(&경준이돈CS2);
		//1. 글로벌 변수 접근(숫자 가져오기)
		//2. 가져온 숫자에 -1
		//3. 다시 글로벌 변수에 저장
		//문열고

	}



	return 0;
}

void STLIncreasement()
{
	for (int i = 0; i < 10000; ++i)
	{
		//경준이돈.fetch_add(1);
	}
}

void STLDecreasement()
{
	for (int i = 0; i < 10000; ++i)
	{

		//경준이돈.fetch_sub(1);
	}
}



//race condition

int main()
{
	//std::thread IncreaseThread1(STLIncreasement);
	//std::thread DecreaseThread1(STLDecreasement);
	//std::thread IncreaseThread2(STLIncreasement);
	//std::thread DecreaseThread2(STLDecreasement);

	//IncreaseThread1.join();
	//DecreaseThread1.join();
	//IncreaseThread2.join();
	//DecreaseThread2.join();


	InitializeCriticalSection(&경준이돈CS1);
	InitializeCriticalSection(&경준이돈CS2);

	DWORD StartTime = timeGetTime();

	//kernel object, signal -> signal, non signal,
	HANDLE ThreadHandles[4];
	ThreadHandles[0] = (HANDLE)_beginthreadex(0, 0, &Increasement, 0, 0, 0);
	ThreadHandles[1] = (HANDLE)_beginthreadex(0, 0, &Decreasement, 0, 0, 0);
	ThreadHandles[2] = (HANDLE)_beginthreadex(0, 0, &Increasement, 0, 0, 0);
	ThreadHandles[3] = (HANDLE)_beginthreadex(0, 0, &Decreasement, 0, 0, 0);
	
	//thread, signal
	WaitForMultipleObjects(4, ThreadHandles, TRUE, INFINITE);

	std::cout << "경준이 돈은?" << 경준이돈 << std::endl;

	std::cout << timeGetTime() - StartTime << std::endl;

	DeleteCriticalSection(&경준이돈CS1);
	DeleteCriticalSection(&경준이돈CS2);

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