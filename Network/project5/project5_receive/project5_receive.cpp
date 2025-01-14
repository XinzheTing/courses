// project5_receive.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <conio.h>
#include <fstream>
#include <iomanip>
#include <time.h>
#include<sys/timeb.h>
#include "pcap.h"
#include <winsock2.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "wpcap.lib")

using namespace std;

typedef unsigned char   u_char;

struct arppkt
{
	unsigned short hdtyp;
	unsigned short protyp;
	unsigned char hdsize;
	unsigned char prosize;
	unsigned short op;
	u_char smac[6];
	u_char sip[4];
	u_char dmac[6];
	u_char dip[4];
};

void packet_handler(const pcap_pkthdr *header, const u_char *pkt_data, ostream &out)
{
	arppkt* arph = (arppkt*)(pkt_data + 14);
	for (int i = 0; i < 3; i++) {
		out << int(arph->dip[i]) << '.';
	}
	out.setf(ios::left);
	out << setw(3) << int(arph->sip[3]) << "  ";
	out.unsetf(ios::left);
	char oldfillchar = out.fill('0');
	out.setf(ios::uppercase);
	for (int i = 0; i < 5; i++)
		out << hex << setw(2) << int(arph->smac[i]) << "-";
	out << hex << setw(2) << int(arph->smac[5]) << "  ";
	out.fill(oldfillchar);
	out.unsetf(ios::hex | ios::uppercase);
	for (int i = 0; i < 3; i++) {
		out << int(arph->dip[i]) << '.';
	}
	out.setf(ios::left);
	out << setw(3) << int(arph->dip[3]) << "  ";
	out.unsetf(ios::left);
	out.fill('0');
	out.setf(ios::uppercase);
	for (int i = 0; i < 5; i++) {
		out << hex << setw(2) << int(arph->dmac[i]) << "-";
	}
	out << hex << setw(2) << int(arph->dmac[5]) << "  ";
	out.fill(oldfillchar);
	out.unsetf(ios::hex | ios::uppercase);
	out << ntohs(arph->op) << "  ";
	struct tm *ltime;
	time_t tmN = header->ts.tv_sec;
	ltime = localtime(&tmN);
	out.fill('0');
	out << ltime->tm_hour << ':' << setw(2) << ltime->tm_min << ':' << setw(2) << ltime->tm_sec;
	out.fill(oldfillchar);
	out << endl;
}

void main(int argc, char *argv[])
{
	if (argc != 2) {
		cout << "Please input command: ParseArp output_file" << endl;
		return;
	}
	pcap_if_t *alldevs;
	pcap_if_t *d;
	pcap_t *adhandle;
	char errbuf[PCAP_ERRBUF_SIZE];
	u_int netmask;
	char packet_filter[] = "ether proto \\arp";
	struct bpf_program fcode;
	struct pcap_pkthdr *header;
	const u_char *pkt_data;
	if (pcap_findalldevs(&alldevs, errbuf) == -1) {
		cout << "Error in pcap_find all devs: " << errbuf;
		return;
	}
	for (d = alldevs; d; d = d->next) {
		if ((adhandle = pcap_open_live(d->name, 1000, 1, 300, errbuf)) == NULL) {
			cout << "Unable to open the adapter.";
			pcap_freealldevs(alldevs);
			return;
		}
		if (pcap_datalink(adhandle) == DLT_EN10MB && d->addresses != NULL)
			break;
	}
	if (d == NULL) {
		cout << "No interfaces found! Make sure Winpcap is installed.";
		return;
	}
	netmask = ((sockaddr_in*)(d->addresses->netmask))->sin_addr.S_un.S_addr;
	if (pcap_compile(adhandle, &fcode, packet_filter, 1, netmask) < 0) {
		cout << "Unable to compile the packet filter. Check the syntax.";
		pcap_freealldevs(alldevs);
		return;
	}
	if (pcap_setfilter(adhandle, &fcode) < 0) {
		cout << "Error setting the filter.";
		pcap_freealldevs(alldevs);
		return;
	}
	cout << "Listening on " << d->description << "..." << endl;
	ofstream fout(argv[1], ios::app);
	time_t t;
	time(&t);
	fout.seekp(0, ios::end);
	int i = fout.tellp();
	if (i!= 0)
		fout << endl;
	fout << "\t\tARP request(1)/reply(2) on " << ctime(&t);
	cout << "Sour IP Addr" << "  " << "Sour MAC Address" << "  " << "Des IP Addr" << "  " << "Des MAC Address" << "  " << "OP"
		<< "  " << "Time" << endl;
	fout << "Sour IP Addr" << "  " << "Sour MAC Address" << "  " << "Des IP Addr" << "  "
		<< "Des MAC Address" << "  " << "OP" << "  " << "Time" << endl;
	pcap_freealldevs(alldevs);
	int result;
	while ((result = pcap_next_ex(adhandle, &header, &pkt_data)) >= 0) {
		if (result == 0)
			continue;
		packet_handler(header, pkt_data, cout);
		packet_handler(header, pkt_data, fout);
	}
}