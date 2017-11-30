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
	//³õÊ¼»¯
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

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	bind(sock, (sockaddr*)&addr, sizeof(sockaddr));
	printf("Now, serer is running on port 6000, waiting for you\n");
	char buf[512];

	while (1)
	{
		memset(buf, 0, sizeof(buf));

		sockaddr_in clientaddr;
		memset(&clientaddr, 0, sizeof(sockaddr_in));

		int clientAddrLen = sizeof(sockaddr);

		//recvfrom»á×èÈû
		int ret = recvfrom(sock, buf, 512, 0, (sockaddr*)&clientaddr, &clientAddrLen);
		printf("Recv msg:%s from IP:[%s] Port:[%d]\n", buf, inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
		sendto(sock, "Good job!", strlen("Good job!"), 0, (sockaddr*)&clientaddr, clientAddrLen);
		printf("Send msg back to IP:[%s] Port:[%d]\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
	}
}