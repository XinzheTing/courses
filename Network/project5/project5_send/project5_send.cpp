// project5_send.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <pcap.h>
#include <string.h>
#include <winsock2.h>
#pragma comment(lib,"wpcap.lib")
#pragma comment(lib,"ws2_32.lib")
using namespace std;

typedef struct PhyFrame
{
	unsigned char DstMAC[6];
	unsigned char SrcMAC[6];
	unsigned short FrameType;
}PHYFRAME;

typedef struct ArpFrame
{
	unsigned short HardwareType;
	unsigned short ProtocolType;
	unsigned char MACLen;
	unsigned char IPLen;
	unsigned short Flag;
	unsigned char SrcMAC[6];
	unsigned char SrcIP[4];
	unsigned char DstMAC[6];
	unsigned char DstIP[4];
	unsigned char Padding[18];
}ARPFRAME;

typedef struct ArpPacket
{
	PHYFRAME phyFrame;
	ARPFRAME arpFrame;
}*ARPPACKET;

ArpPacket arpPacket;

int CheckMAC(char *argv, unsigned char *mac_addr)
{
	char mac[18];
	int i = 0, j = 0, k = 0;
	strcpy_s(mac, argv);
	int m = strlen(argv);

	for (i = 0; i < m; i++) {
		if (mac[i]<'-' || (mac[i]>'-'&&mac[i]<'0') || (mac[i]>'9'&&mac[i]<'A') || (mac[i]>'Z'&&mac[i]<'a') || mac[i]>'z') {
			cout << "MAC address errol!" << endl;
			return 0;
		}
		else
		{
			if (mac[i] != '-')
				j++;
			else
			{
				k++;
				if (j > 2)
				{
					cout << "MAC address errol!" << endl;
					return 0;
				}
				else
					j = 0;
				if (k > 5)
				{
					cout << "MAC address errol!" << endl;
					return 0;
				}
			}
		}
	}
	for (i = 0; i < 6; i++)
	{
		while (*(mac + j) == '-')
			j++;
		if (*(mac + j) >= '0'&&*(mac + j) <= '9')
			mac_addr[i] = (unsigned char)(*(mac + j) - '0');
		if (*(mac + j) >= 'a'&&*(mac + j) <= 'z')
			mac_addr[i] = (unsigned char)(*(mac + j) - 'a' + 10);
		if (*(mac + j) >= 'A'&&*(mac + j) <= 'Z')
			mac_addr[i] = (unsigned char)(*(mac + j) - 'A' + 10);
		j++;
		mac_addr[i] *= 16;
		if (*(mac + j) >= '0'&&*(mac + j) <= '9')
			mac_addr[i] += (unsigned char)(*(mac + j) - '0');
		if (*(mac + j) >= 'a'&&*(mac + j) <= 'z')
			mac_addr[i] += (unsigned char)(*(mac + j) - 'a' + 10);
		if (*(mac + j) >= 'A'&&*(mac + j) <= 'Z')
			mac_addr[i] += (unsigned char)(*(mac + j) - 'A' + 10);
		j++;
	}
	return 1;
}
int CheckIP(char *argv, unsigned char *ip_addr)
{
	char ip[16];
	int i = 0, j = 0, k = 0;
	strcpy_s(ip, argv);
	int m = strlen(argv);

	for (i = 0; i < m; i++) {
		if (ip[i]<'.' || (ip[i]>'.'&&ip[i]<'0') || ip[i]>'9')
		{
			cout << "IP address errol!" << endl;
			return 0;
		}
		else
		{
			if (ip[i] != '.')
				j++;
			else
			{
				k++;
				if (j > 3)
				{
					cout << "IP address errol!" << endl;
					return 0;
				}
				else
					j = 0;
				if (k > 3)
				{
					cout << "IP address errol!" << endl;
					return 0;
				}
			}
		}
	}
	for (i = 0; i < 4; i++)
	{
		while (*(ip + j) == '.')
			j++;
		ip_addr[i] = (unsigned char)atoi(ip + j);
	}
	return 1;
}
void main(int argc, char *argv[])
{
	if (argc != 5)
	{
		cout << "Please input command: SendArp source_ip source_mac dest_ip dest_mac" << endl;
		return;
	}

	int i = 0;
	memset(&arpPacket, 0, sizeof(arpPacket));
	if (!CheckMAC(argv[4], arpPacket.phyFrame.DstMAC))
		return;
	if (!CheckMAC(argv[2], arpPacket.phyFrame.SrcMAC))
		return;
	arpPacket.phyFrame.FrameType = htons((unsigned short)0x0608);

	arpPacket.arpFrame.HardwareType = (unsigned short)0x0100;
	arpPacket.arpFrame.ProtocolType = (unsigned short)0x0008;
	arpPacket.arpFrame.MACLen = (unsigned char)6;
	arpPacket.arpFrame.IPLen = (unsigned char)4;
	if (!CheckMAC(argv[2], arpPacket.arpFrame.SrcMAC))
		return;
	if (!CheckIP(argv[1], arpPacket.arpFrame.SrcIP))
		return;
	if (!CheckMAC(argv[4], arpPacket.arpFrame.DstMAC))
		return;
	if (!CheckIP(argv[3], arpPacket.arpFrame.DstIP))
		return;
	for (i = 0; i < 18; i++)
		arpPacket.arpFrame.Padding[i] = 0;

	pcap_if_t *alldevs;
	pcap_if_t *d, *head = NULL;
	pcap_t *fp;
	char errbuf[PCAP_ERRBUF_SIZE];

	if (pcap_findalldevs(&alldevs, errbuf) == -1)
	{
		cout << "Unable to create adapter list!" << endl;
		return;
	}

	i = 0;
	for (d = alldevs; d; d = d->next)
	{
		cout << ++i << ": " << d->name;
		if (d->description)
			cout << " " << d->description << endl;
	}

	if (i == 0)
	{
		cout << "No adapter found!" << endl;
		return;
	}

	cout << "Enter the interface number(1-" << i << "):";
	int k;
	cin >> k;
	if (k<1 || k>i)
	{
		cout << "Out of range!" << endl;
		return;
	}
	for (d = alldevs, i = 1; i < k; d = d->next, i++)
		head = d;

	if ((fp = pcap_open_live(head->name, 1000, 1, 1000, errbuf)) == NULL)
	{
		cout << "Unable to open the adapter!" << endl;
		pcap_freealldevs(alldevs);
		return;
	}

	if (pcap_sendpacket(fp, (unsigned char*)&arpPacket, sizeof((unsigned char*)&arpPacket)) == -1)
	{
		cout << "ARP packet send error!" << endl;
		return;
	}

	cout << "Source MAC: " << argv[1] << endl;
	cout << "Source IP: " << argv[2] << endl;
	cout << "Dest MAC: " << argv[3] << endl;
	cout << "Dest IP: " << argv[4] << endl;
	cout << "ARP packet send success!" << endl;
}