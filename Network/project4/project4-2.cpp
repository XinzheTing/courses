// project4.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#pragma comment(lib,"ws2_32.lib")
#define BUFFER_SIZE 65536
#define IO_RCVALL _WSAIOW(IOC_VENDOR, 1)
//定义IP首部数据结构
typedef  struct ip_header
{
	union
	{
		BYTE version;   //版本号4（前4比特位）
		BYTE HeaderLen;  //IP首部长度（后4比特位）  
	};
	BYTE ServiceType;       //服务类型
	WORD TotalLen;          //总长度
	WORD ID;                // 标识
	union {
		WORD  Flags; //标志（前3比特位）
		WORD  FragOff;   //分片偏移（后13比特位）
	};
	BYTE TimeToLive;       //生存期
	BYTE Protocol;         //协议类型
	WORD  HdrChsum;        //16比特位校验和
	DWORD SrcAddr;         //32比特源IP地址
	DWORD DstAddr;         //32比特目的IP地址
	BYTE options;          //选项 
} IP_HEADER;
using namespace std;
//解析IP包的版本信息
void getVersion(BYTE b, BYTE &version)
{
	version = b >> 4;
}
//解析IP包的头部长度
void getIHL(BYTE b, BYTE &length)
{
	length = (b & 0x0f) * 4;
}
//解析IP包的服务类型
char* parseServiceType_getProcedence(BYTE b)
{
	switch (b >> 5)
	{
	case 7:
		return "Network Control";
		break;
	case 6:
		return "Internrt work Control";
		break;
	case 5:
		return "CRITIC/ECP";
		break;
	case 4:
		return "Flash Override";
		break;
	case 3:
		return "Flash";
		break;
	case 2:
		return "Immediate";
		break;
	case 1:
		return "Priority";
		break;
	case 0:
		return "Routine";
		break;
	default:
		return "Unknown";

	}
}
//解析IP报的服务级别
char* parseServiceType_getTOS(BYTE b)
{
	b = (b >> 1) & 0x0f;
	switch (b)
	{
	case 0:
		return "Normal service";
		break;
	case 1:
		return "Minimize monetary cost";
		break;
	case 2:
		return "Maxmize reliability";
		break;
	case 4:
		return "Maxmize throughput";
		break;
	case 8:
		return "Minimize delay";
		break;
	case 15:
		return "Maxmize security";
		break;
	default:
		return "Unknown";
	}
}
void getFlags(WORD w, BYTE &DF, BYTE &MF)
{
	DF = (w >> 14) & 0x01;
	MF = (w >> 13) & 0x01;
}
//解析IP包的分段偏移
void getFragoff(WORD w, WORD &fragoof)
{
	fragoof = w & 0x1fff;
}
//解析IP包的协议类型
char *getProtocol(BYTE Protocol)
{
	switch (Protocol)
	{
	case 1:
		return "ICMP";
	case 2:
		return "IGMP";
	case 4:
		return "ip in ip";
	case 6:
		return "TCP";
	case 8:
		return"EGP";
	case 17:
		return "UDP";
	case 41:
		return"IPV6";
	case 46:
		return "RSVP";
	case 89:
		return "OSPF";
	default:
		return"Unknown";
	}
}

void ipparse(FILE *file, char *buffer)
{
	IP_HEADER ip = *(IP_HEADER*)buffer;
	fseek(file, 0, SEEK_END);
	fprintf(file, "--------------------\n");

	//解析IP包的版本信息
	BYTE version;
	getVersion(ip.version, version);
	fprintf(file, "version:%d\n", version);

	//解析IP包的头部长度
	BYTE headerLen;
	getIHL(ip.HeaderLen, headerLen);
	fprintf(file, "HdrLen: %d(Bytes)\n", headerLen);

	//解析IP包的服务类型与等级
	fprintf(file, "serviceType:%s,%s\n", parseServiceType_getProcedence(ip.ServiceType), parseServiceType_getTOS(ip.ServiceType));

	//解析IP包的总长度
	fprintf(file, "TotalLen:%d(Byte)\n", ip.TotalLen);

	//解析IP包的标识符
	fprintf(file, "ID:%d\n", ip.ID);

	//解析IP包的标志位
	BYTE DF, MF;
	getFlags(ip.Flags, DF, MF);
	fprintf(file, "Flags:DF=%d,MF=%d\n", DF, MF);

	//解析IP包的分段偏移
	WORD fragoff;
	getFragoff(ip.FragOff, fragoff);
	fprintf(file, "Fragoff:%d\n", fragoff);

	//解析IP包的生存期
	fprintf(file, "TimeToLive:%d(Hops)\n", ip.TimeToLive);

	//解析IP包的协议类型
	fprintf(file, "Protocol:%s\n", getProtocol(ip.Protocol));

	//解析IP包的头部校验和
	fprintf(file, "HdrChksum:0x%0x\n", ip.HdrChsum);

	//解析IP包的源IP地址
	fprintf(file, "SrcAddr:%s\n", inet_ntoa(*(in_addr*)&ip.SrcAddr));

	//解析IP包的目的IP地址
	fprintf(file, "DstAddr:%s\n", inet_ntoa(*(in_addr*)&ip.DstAddr));

}
void main(int argc, char* argv[])
{
	//检查输入命令格式
	if (argc != 2)
	{
		printf("please input correct parameter : exe logfile\n");
		return;
	}
	// 打开输出日志文件
	FILE * file;
	if ((file = fopen(argv[1], "wb + ")) == NULL)
	{
		printf("fail to open file %s", argv[1]);
		return;
	}

	// 初始化Socket 
	WSADATA wsData;
	if (WSAStartup(MAKEWORD(2, 2), &wsData) != 0)
	{
		printf("WSAStartup  failed\n");
		return;
	}

	//建立原始Socket
	SOCKET sock;
	if ((sock = socket(AF_INET, SOCK_RAW, IPPROTO_IP)) == INVALID_SOCKET)
	{
		printf("create socket failed\n"); \
			return;
	}

	// 设置IP首部操作选项
	BOOL flag = true;
	if (setsockopt(sock, IPPROTO_IP, IP_HDRINCL, (char *)&flag, sizeof(flag)) == SOCKET_ERROR)
	{
		printf("setsocket failed\n");
	}

	//获取本地计算机名称
	char hostname[128];
	if (gethostname(hostname, 100) == SOCKET_ERROR)
	{
		printf("gethost name failed\n");
		return;
	}

	//获得本地主机IP地址
	hostent *pHostIP;
	if ((pHostIP = gethostbyname(hostname)) == NULL)
	{
		printf("gethostbyname failed\n");
		return;
	}

	//填充SOCKADD_IN 结构
	sockaddr_in  addr_in;
	addr_in.sin_addr = *(in_addr*)pHostIP->h_addr_list[0];
	addr_in.sin_family = AF_INET;
	addr_in.sin_port = htons(6000);

	//将原始SOCKET绑定到本地网卡
	if (bind(sock, (PSOCKADDR)&addr_in, sizeof(addr_in)) == SOCKET_ERROR)
	{
		printf("bind failed\n");
		return;
	}

	//设置SOCK_RAW 为IO_RCVALL, 接收所有IP分组
	DWORD dwValue = 1;
	DWORD dwBufferLen[10];
	DWORD dwBufferInLen = 1;
	DWORD dwBytesReturned = 0;
	if (WSAIoctl(sock, IO_RCVALL, &dwBufferInLen, sizeof(dwBufferInLen), &dwBufferLen, sizeof(dwBufferLen), &dwBytesReturned, NULL, NULL) == SOCKET_ERROR)
	{
		printf("Ioctlsocket failed\n");
		return;
	}
	//捕获经过本机的所有IP分组
	char buffer[BUFFER_SIZE];
	printf("listening on local host….\n");
	while (true)
	{
		int size = recv(sock, buffer, BUFFER_SIZE, 0);
		if (size > 0)
		{
			ipparse(stdout, buffer);   // 解析结果输出屏幕
			ipparse(file, buffer);         // 解析结果输出到文件          
		}
	}
	fclose(file); return;
}

