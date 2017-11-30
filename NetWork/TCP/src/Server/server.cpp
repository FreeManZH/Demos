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
	//��ʼ��
	if (!InitNetEnv())
	{
		return -1;
	}

	//�׽��֣������ڹܵ�,
	//��һ����������ʾҪѡ��ip�汾
	//��������������
	//��������Э������
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
	{
		printf("socket failed\n");
		return -1;
	}
	printf("socket create successed\n");

	//��һ����ַ�Ͷ˿ڣ����Կ����ж�����������ж����ַ��������Ҫ��һ����ַ
	SOCKADDR_IN addrServ;
	addrServ.sin_family = AF_INET;	//�����
	addrServ.sin_port = htons(PORT);	//�˿ڣ���Ҫת�������ֽ���
	addrServ.sin_addr.S_un.S_addr = INADDR_ANY;
	int ret = bind(sock, (sockaddr*)&addrServ, sizeof(addrServ));
	if (SOCKET_ERROR == ret)
	{
		printf("socket bind failed\n");
		WSACleanup();	//�ͷ����绷��
		closesocket(sock);	//�ر���������
		return -1;
	}
	printf("socket bind successed\n");

	ret = listen(sock, SOMAXCONN);	//���� �ȴ����Ӷ��еĳ��� - �����ܵĿ��ٽ��ܿͻ�������
	if (SOCKET_ERROR == ret)
	{
		printf("socket listen failed\n");
		WSACleanup();	//�ͷ����绷��
		closesocket(sock);	//�ر���������
		return -1;
	}
	printf("socket listen successed\n");

	sockaddr_in addrClient;
	int addrClientLen = sizeof(sockaddr_in);
	while (1)
	{
		// TCP ÿ��һ���ͣ���ȥ�´���һ��SOCKET�׽���
		SOCKET client = accept(sock, (sockaddr*)&addrClient, &addrClientLen);
		if (INVALID_SOCKET == client)
		{
			printf("socket accept failed\n");
			WSACleanup();	//�ͷ����绷��
			closesocket(sock);
			return -1;
		}
		printf("socket accept successed\n");
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)clientProc, (LPVOID)client, 0, 0);
	}

	return 0;
}
