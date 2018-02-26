#include "pin.H"
#include <stdio.h>

char* getIPFromInt(int ip) {
    unsigned char bytes[4];
    bytes[0] = ip & 0xFF;
    bytes[1] = (ip >> 8) & 0xFF;
    bytes[2] = (ip >> 16) & 0xFF;
    bytes[3] = (ip >> 24) & 0xFF;  
	char *ipBuf = (char*) calloc(13, sizeof(char));
    sprintf(ipBuf, "%d.%d.%d.%d\0", bytes[3], bytes[2], bytes[1], bytes[0]);        
	return ipBuf;
}

void printRegistersState(CONTEXT* ctx) {
	fprintf(stdout, "eax: 0x%lx\nebx: 0x%lx\necx: 0x%lx\nedx: 0x%lx\nsp: 0x%lx\nebp: 0x%lx\nesi: 0x%lx\nedi: 0x%lx\n",
	PIN_GetContextReg(ctx, REG_EAX),
	PIN_GetContextReg(ctx, REG_EBX),
	PIN_GetContextReg(ctx, REG_ECX),
	PIN_GetContextReg(ctx, REG_EDX),
	PIN_GetContextReg(ctx, REG_ESP),
	PIN_GetContextReg(ctx, REG_EBP),
	PIN_GetContextReg(ctx, REG_ESI),
	PIN_GetContextReg(ctx, REG_EDI));
	//PIN_GetContextReg(ctx, REG_R8),
	//PIN_GetContextReg(ctx, REG_R9));
}