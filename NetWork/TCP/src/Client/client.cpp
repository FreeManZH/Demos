#include <stdio.h>
#include <WinSock2.h>
#include <windows.h>


#pragma comment(lib, "ws2_32.lib")
#define PORT 6000

bool InitNetEnv()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("WSAStartup failed\n");
		return false;
	}
	return true;
}

int main(int argc, char* argv[])
{
	//初始化
	if (!InitNetEnv())
	{
		return -1;
	}

	//套接字，类似于管道,
	//第一个参数：表示要选择ip版本
	//参数二：流类型
	//参数三：协议类型
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
	{
		printf("socket failed\n");
		return -1;
	}
	printf("socket socket successed\n");

	SOCKADDR_IN addrServ;
	addrServ.sin_family = AF_INET;
	addrServ.sin_port = htons(PORT);
	addrServ.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	int ret = connect(sock, (SOCKADDR*)&addrServ, sizeof(SOCKADDR));
	if (SOCKET_ERROR == ret)
	{
		printf("socket connect failed");
		WSACleanup();
		closesocket(sock);
		return -1;
	}
	printf("socket connect successed\n");

	char szBuf[512];
	memset(szBuf, 0, sizeof(szBuf));
	sprintf_s(szBuf, sizeof(szBuf), "hello world");

	ret = send(sock, szBuf, sizeof(szBuf), 0);
	if (SOCKET_ERROR == ret)
	{
		printf("socket send failed\n");
		closesocket(sock);
		return -1;
	}
	printf("socket send successed\n");

	ret = recv(sock, szBuf, sizeof(szBuf), 0);
	if (SOCKET_ERROR == ret)
	{
		printf("socket recv failed\n");
		closesocket(sock);
		return -1;
	}
	printf("socket recv successed\n");
	printf("%s\n", szBuf);
	closesocket(sock);
	WSACleanup();
	return 0;
}