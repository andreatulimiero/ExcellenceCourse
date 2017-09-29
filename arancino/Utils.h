#include "pin.H"
#include <stdio.h>
/* Network Utils */
#define HTONS(n) (((((unsigned short)(n) & 0xFF)) << 8) | (((unsigned short)(n) & 0xFF00) >> 8))
#define NTOHS(n) HTONS(n)

#define HTONL(n) (((((unsigned long)(n) & 0xFF)) << 24) | \
                  ((((unsigned long)(n) & 0xFF00)) << 8) | \
                  ((((unsigned long)(n) & 0xFF0000)) >> 8) | \
                  ((((unsigned long)(n) & 0xFF000000)) >> 24))
#define NTOHL(n) HTONL(n)

char* getIPFromInt(int ip);
/* END Network Utils */


void printRegistersState(CONTEXT* ctx);