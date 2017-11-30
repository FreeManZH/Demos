#include <iostream>
#include <WinSock2.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

#define TargetPort 2425
#define TargetIPAddress "192.168.1.106"
#define TreatIPAddress "127.0.0.1"
#define IP_HDRINCL 2

typedef struct ip_hdr
{
	unsigned char ip_verlen;
	unsigned char ip_tos;
	unsigned short ip_totallength;
	unsigned short ip_id;
	unsigned short ip_offset;
	unsigned char ip_ttl;
	unsigned char ip_protocol;
	unsigned short ip_checksum;
	unsigned int ip_srcaddr;
	unsigned int ip_destaddr;
}IP_HDR;

typedef struct udp_hdr
{
	unsigned short src_portno;
	unsigned short dst_portno;
	unsigned short udp_length;
	unsigned short udp_checksum;
}UDP_HDR;

bool winsockinit()
{
	WSAData wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		cout << "WSAStartup() failed..." << endl;
		return false;
	}
	return true;
}

USHORT checksum(USHORT *buffer, int size)
{
	unsigned long cksum = 0;
	while (size > 1)
	{
		cksum += *buffer++;
		size -= sizeof(USHORT);
	}
	if (size)
	{
		cksum += *(UCHAR*)buffer;
	}
	cksum = (cksum >> 16) + (cksum & 0xffff);
	cksum += (cksum >> 16);
	return (USHORT)(~cksum);
}

int main(int argc, char *argv[])
{
	cout << "Start init winsock...." << endl;
	if (winsockinit())
		cout << "winsock init success..." << endl;
	SOCKET sock = WSASocket(AF_INET, SOCK_RAW, IPPROTO_UDP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (sock == INVALID_SOCKET)
	{
		cout << "WSASocket() failed:" << WSAGetLastError() << endl;
		return -1;
	}

	DWORD bOpt = true;
	int ret = setsockopt(sock, IPPROTO_IP, IP_HDRINCL, (const char*)&bOpt, sizeof(bOpt));
	if (ret == SOCKET_ERROR)
	{
		cout << "setsockopt() failed:" << WSAGetLastError() << endl;
		return -1;
	}
	const int buffer_size = 256;
	char buffer[buffer_size];
	const char* strMessage = "1:1447414735:It's not your biz:ZS-EDU:32:Test Treatment Message";
	IP_HDR ipHdr;

	const unsigned short iIPSize = sizeof(ipHdr);
	const unsigned short iIPVersion = 4;
	ipHdr.ip_verlen = (4 << 4 | (sizeof(IP_HDR) / sizeof(ULONG)));
	ipHdr.ip_tos = 0;
	const unsigned short iTotalSize = sizeof(ipHdr)+sizeof(UDP_HDR)+strlen(strMessage);
	ipHdr.ip_totallength = htons(iTotalSize);
	ipHdr.ip_id = 0;
	ipHdr.ip_offset = 0;
	ipHdr.ip_ttl = 128;
	ipHdr.ip_protocol = IPPROTO_UDP;
	ipHdr.ip_destaddr = inet_addr(TargetIPAddress);
	ipHdr.ip_srcaddr = inet_addr(TreatIPAddress);
	ipHdr.ip_checksum = checksum((USHORT*)&ipHdr, sizeof(IP_HDR));
	UDP_HDR udpHdr;
	udpHdr.dst_portno = htons(TargetPort);
	const u_short uFromPort = 1000;
	udpHdr.src_portno = htons(uFromPort);

	const unsigned short iUdpSize = sizeof(udpHdr)+strlen(strMessage);
	udpHdr.udp_length = htons(iUdpSize);
	udpHdr.udp_checksum = 0;

	RtlZeroMemory(buffer, buffer_size);
	char *ptr = buffer;
	memcpy(ptr, &ipHdr, sizeof(ipHdr));

	ptr += sizeof(ipHdr);
	memcpy(ptr, &udpHdr, sizeof(udpHdr));

	ptr += sizeof(udpHdr);
	memcpy(ptr, strMessage, strlen(strMessage));

	sockaddr_in remote;
	remote.sin_family = AF_INET;
	remote.sin_port = htons(TargetPort);
	remote.sin_addr.s_addr = inet_addr("127.0.0.5");

	printf("TO %s:%d ", TargetIPAddress, TargetPort);
	ret = sendto(sock, buffer, iTotalSize, 0, (SOCKADDR*)&remote, sizeof(remote));

	if (ret == SOCKET_ERROR)
	{
		printf("sendto() failed: %d ", WSAGetLastError());
	}
	else
		printf("send %d bytes ", ret);
	closesocket(sock);
	WSACleanup();
	system("pause");
	return 0;
}