#include <WinSock2.h>
#include <windows.h>
#include <stdio.h>

#define PORT 6000
#pragma comment(lib, "ws2_32.lib")
fd_set g_fdClientSock;
int clientNum = 0;

BOOL WinSockInit()
{
	WSADATA wsaData = { 0 };
	if (WSAStartup(MAKEWORD(2, 2), &wsaData))
		return FALSE;
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		WSACleanup();
		return FALSE;
	}
	return TRUE;
}

DWORD WINAPI ListenThreadProc(LPARAM lparam)
{
	fd_set fdRead;
	FD_ZERO(&fdRead);
	int nRet = 0;
	char* recvBuffer = (char*)malloc(sizeof(char)* 1024);
	if (recvBuffer == NULL)
		return -1;
	memset(recvBuffer, 0, sizeof(char)* 1024);
	while (1)
	{
		fdRead = g_fdClientSock;
		// select �����������������FD_SET�������������socket�Ƿ����ź�
		nRet = select(0, &fdRead, NULL, NULL, NULL);
		if (nRet != SOCKET_ERROR)
		{
			for (int i = 0; i < g_fdClientSock.fd_count; i++)
			{
				if (FD_ISSET(g_fdClientSock.fd_array[i], &fdRead))
				{
					memset(recvBuffer, 0, sizeof(char)* 1024);
					nRet = recv(g_fdClientSock.fd_array[i], recvBuffer, 1024, 0);
					if (nRet == SOCKET_ERROR)
					{
						closesocket(g_fdClientSock.fd_array[i]);
						--clientNum;
						FD_CLR(g_fdClientSock.fd_array[i], &g_fdClientSock);
					}
					else if (nRet == 0)
					{
						closesocket(g_fdClientSock.fd_array[i]);
						--clientNum;
						FD_CLR(g_fdClientSock.fd_array[i], &g_fdClientSock);
					}
					else
					{
						printf("Recv msg:%s\n", recvBuffer);
						send(g_fdClientSock.fd_array[i], recvBuffer, strlen(recvBuffer), 0);
					}
				}
			}
		}
	}
}

int main()
{
	WinSockInit();

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
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ListenThreadProc, NULL, NULL, NULL);

	while (clientNum < FD_SETSIZE)
	{
		SOCKET clientSock = accept(sock, (sockaddr*)&addrClient, &addrClientLen);
		FD_SET(clientSock, &g_fdClientSock);
		clientNum++;
	}
	closesocket(sock);
	WSACleanup();
	return 0;
}