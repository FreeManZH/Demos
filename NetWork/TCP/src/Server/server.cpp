#include <stdio.h>
#include <WinSock2.h>
#include <Windows.h>

#pragma comment(lib, "Ws2_32.lib")
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
DWORD WINAPI clientProc(LPARAM lparam)
{
	SOCKET client = (SOCKET)lparam;
	char buf[1024];
	while (1)
	{
		memset(buf, 0, sizeof(1024));
		int ret = recv(client, buf, sizeof(buf), 0);
		if (SOCKET_ERROR == ret)
		{
			printf("socket recv failed");
			closesocket(client);
			return -1;
		}
		if (ret == 0)
		{
			printf("socket close connection\n");
			closesocket(client);
			return -1;
		}
		printf("Recv data[ %s ]", buf);
		ret = send(client, buf, strlen(buf), 0);
		if (SOCKET_ERROR == ret)
		{
			printf("socket send failed\n");
			return -1;
		}
	}
	closesocket(client);
	return 0;
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
	printf("socket create successed\n");

	//绑定一个地址和端口，电脑可能有多个网卡，就有多个地址，所以需要绑定一个地址
	SOCKADDR_IN addrServ;
	addrServ.sin_family = AF_INET;	//网络簇
	addrServ.sin_port = htons(PORT);	//端口，需要转换网络字节序
	addrServ.sin_addr.S_un.S_addr = INADDR_ANY;
	int ret = bind(sock, (sockaddr*)&addrServ, sizeof(addrServ));
	if (SOCKET_ERROR == ret)
	{
		printf("socket bind failed\n");
		WSACleanup();	//释放网络环境
		closesocket(sock);	//关闭网络连接
		return -1;
	}
	printf("socket bind successed\n");

	ret = listen(sock, SOMAXCONN);	//监听 等待连接队列的长度 - 尽可能的快速接受客户端连接
	if (SOCKET_ERROR == ret)
	{
		printf("socket listen failed\n");
		WSACleanup();	//释放网络环境
		closesocket(sock);	//关闭网络连接
		return -1;
	}
	printf("socket listen successed\n");

	sockaddr_in addrClient;
	int addrClientLen = sizeof(sockaddr_in);
	while (1)
	{
		// TCP 每接一个客，会去新创建一个SOCKET套接字
		SOCKET client = accept(sock, (sockaddr*)&addrClient, &addrClientLen);
		if (INVALID_SOCKET == client)
		{
			printf("socket accept failed\n");
			WSACleanup();	//释放网络环境
			closesocket(sock);
			return -1;
		}
		printf("socket accept successed\n");
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)clientProc, (LPVOID)client, 0, 0);
	}

	return 0;
}
