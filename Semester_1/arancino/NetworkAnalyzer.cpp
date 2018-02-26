#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>

#include <list>
#include "pin.H"

namespace W {
	#include <WinSock2.h>
	#include <WS2tcpip.h>
	#include <Windows.h>
}

#include "Utils.h"

namespace NetworkAnalyzer {

unsigned int connect_callno = 0; // global connect call number
typedef struct {
	unsigned short s_port;
	char* s_ip_addr;
	unsigned int callno; // storing the call order to be able to link with send report
} connect_report;
list<connect_report*> connect_report_list;

typedef struct {
	unsigned int bytes_sent;
	char* msg;
	unsigned int callno;
} send_report;
list<send_report*> send_report_list;

VOID connectCallHandler(ADDRINT s, ADDRINT sockaddr_ptr, ADDRINT sockaddr_len) {
	/*printf("Args: Socket:%d - Socket Address:0x%xd - Socket size:%d \n",
		s,
		sockaddr_ptr,
		sockaddr_len
	);*/
	W::sockaddr_in sock_in; memcpy(&sock_in, (void*) sockaddr_ptr, sizeof(W::sockaddr_in));
	printf("Socket family: %hu, PORT: %hu\n", sock_in.sin_family, NTOHS(sock_in.sin_port));
	char* ip_addr;
	ip_addr = getIPFromInt(NTOHL(sock_in.sin_addr.S_un.S_addr));
	printf("IP Address: %s\n", ip_addr);
	connect_report *report = new connect_report;
	report->s_ip_addr = ip_addr;
	report->s_port = NTOHS(sock_in.sin_port);
	report->callno = ++connect_callno;
	connect_report_list.push_back(report);
}

VOID sendCallHandler(ADDRINT s, ADDRINT buf, UINT len, ADDRINT flags) {
	printf("Sending %d bytes: %s\n", len, buf);
	send_report *report = new send_report;
	report->bytes_sent = len;
	report->msg = (char*) calloc(len + 1, sizeof(char));
	memcpy(report->msg, (void*) buf, len);
	report->callno = connect_callno;
	send_report_list.push_back(report);
}

VOID recvCallHandler(ADDRINT s, ADDRINT buf, UINT len, ADDRINT flags) {
	printf("Received %d bytes: %s\n", len, buf);
}

VOID Routine(RTN rtn, VOID* v) {
	string name = RTN_Name(rtn);
	if (_stricmp(name.c_str(), "connect") == 0) {
		RTN_Open(rtn);

		RTN_InsertCall(rtn,
			IPOINT_BEFORE,
			(AFUNPTR)connectCallHandler,
			IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
			IARG_FUNCARG_ENTRYPOINT_VALUE, 1,
			IARG_FUNCARG_ENTRYPOINT_VALUE, 2,
			IARG_END);
		
		RTN_Close(rtn);
	} else if (_stricmp(name.c_str(), "send") == 0) {
		
		RTN_Open(rtn);

		RTN_InsertCall(rtn,
			IPOINT_AFTER,
			(AFUNPTR)sendCallHandler,
			IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
			IARG_FUNCARG_ENTRYPOINT_VALUE, 1,
			IARG_FUNCARG_ENTRYPOINT_VALUE, 2,
			IARG_FUNCARG_ENTRYPOINT_VALUE, 4,
			IARG_END);
		
		RTN_Close(rtn);
	} else if (_stricmp(name.c_str(), "recv") == 0) {
		
		RTN_Open(rtn);

		RTN_InsertCall(rtn,
			IPOINT_BEFORE,
			(AFUNPTR)recvCallHandler,
			IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
			IARG_FUNCARG_ENTRYPOINT_VALUE, 1,
			IARG_FUNCARG_ENTRYPOINT_VALUE, 2,
			IARG_FUNCARG_ENTRYPOINT_VALUE, 4,
			IARG_END);
		
		RTN_Close(rtn);
	}
}

VOID Fini(string base_path) {
	list<connect_report*>::iterator c_i = connect_report_list.begin();
	string path = base_path + "network_analysis.txt"; 
	FILE* f = fopen(path.c_str(), "w+");
	for (; c_i != connect_report_list.end(); c_i++) {
		fprintf(f, "[Connect]{%d} %s:%d\n", (*c_i)->callno, (*c_i)->s_ip_addr, (*c_i)->s_port);
		free(*c_i);
	}
	
	list<send_report*>::iterator s_i = send_report_list.begin();
	for (; s_i != send_report_list.end(); s_i++) {
		fprintf(f, "[Send]{%d} (%dbytes)%s\n", (*s_i)->callno, (*s_i)->bytes_sent, (*s_i)->msg);
		free(*c_i);
	}
	fflush(f);
	fclose(f);
	printf("Saved network report to %s\n", path);
}

}