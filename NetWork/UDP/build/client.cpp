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

	SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (SOCKET_ERROR == sock)
	{
		printf("create socket failed\n");
		return -1;
	}

	//客户端通常不需要绑定端口，但是也可以指定发送端口
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT + 1);
	addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	bind(sock, (sockaddr*)&addr, sizeof(sockaddr));

	//sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	char buf[] = "hello, i'm client";
	int dwSent = sendto(sock, buf, sizeof(buf), 0, (SOCKADDR*)&addr, sizeof(SOCKADDR));
	if (dwSent == 0)
	{
		printf("send %s failed\n", buf);
		return -1;
	}

	printf("send msg:%s\n", buf);

	char recvBuf[512];
	memset(recvBuf, 0, 512);

	//会保存从服务器接收消息的“客户地址”
	sockaddr_in addrServer = { 0 };
	int nServerAddrLen = sizeof(addrServer);
	dwSent = recvfrom(sock, recvBuf, 512, 0, (SOCKADDR*)&addrServer, &nServerAddrLen);
	printf("Recv msg from server : %s\n", recvBuf);

	closesocket(sock);
	WSACleanup();
	system("pause");
	return 0;
}