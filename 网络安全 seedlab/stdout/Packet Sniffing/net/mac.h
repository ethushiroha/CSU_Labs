#ifndef _MAC_H_
#define _MAC_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

    // mac
#define MAC_ADDRESS_LENGTH          6
#define MAC_TYPE                    2

int MAC_LENGTH = 0;

typedef struct { // MAC
    unsigned char dst[MAC_ADDRESS_LENGTH];
    unsigned char src[MAC_ADDRESS_LENGTH];
    unsigned char type[MAC_TYPE];
} MAC;

int getMacLength() {
    MAC_LENGTH = 14;
    return 14;
}

MAC *getMac(char *buf) {
    MAC *mac = (MAC*)malloc(sizeof(MAC));
    getMacLength();
    memcpy(mac, buf, MAC_LENGTH);
    return mac;
}

char *getDstMacAddress(MAC *mac) {
    unsigned char *dst = mac->dst;
    char *dstMac = (char*)malloc(sizeof(char) * 0x18);
    memset(dstMac, 0, 0x18);
    sprintf(dstMac, "%2x:%2x:%2x:%2x:%2x:%2x", dst[0], dst[1], dst[2], dst[3], dst[4], dst[5]);
    // printf("mac: %s\n", dstMac);
    return dstMac;
}

char *getSrcMacAddress(MAC *mac) {
    unsigned char *src = mac->src;
    char *srcMac = (char*)malloc(sizeof(char) * 0x18);
    memset(srcMac, 0, 0x18);
    sprintf(srcMac, "%2x:%2x:%2x:%2x:%2x:%2x", src[0], src[1], src[2], src[3], src[4], src[5]);
    // printf("mac: %s\n", srcMac);
    return srcMac;
}

#endif // _MAC_H_