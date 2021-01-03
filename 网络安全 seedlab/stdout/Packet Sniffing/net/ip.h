#ifndef _IP_H_
#define _IP_H_

#include "mac.h"

// ip
#define IP_VERSION_LENGTH           1
#define IP_QOS_LENGTH               1
#define IP_TOTAL_LENGTH             2
#define IP_IDENTIFICATION_LENGTH    2
#define IP_FLAG_LENGTH              2
#define IP_TTL                      1
#define IP_PROTOCOL                 1
#define IP_HDEADER_ADD              2
#define IP_ADDRESS_LENGTH           4
#define IP_PADDING                  20

enum protocols{
    TCPPROTOCOL = 100,
    UDPPROTOCOL,
    ICMPPROTOCOL
};

int IP_LENGTH = 0;
int PACKET_LENGTH = 0;
int DATA_LENGTH = 0;

typedef struct { // IP
    // MAC mac_header;
    unsigned char version_header_length[IP_VERSION_LENGTH];
    unsigned char QOS[IP_QOS_LENGTH];
    unsigned char ip_total_length[IP_TOTAL_LENGTH];
    unsigned char identification[IP_IDENTIFICATION_LENGTH];
    unsigned char flag_fragmen[IP_FLAG_LENGTH];
    unsigned char ttl[IP_TTL];
    unsigned char protocol[IP_PROTOCOL];
    unsigned char header_add[IP_HDEADER_ADD];
    unsigned char src[IP_ADDRESS_LENGTH];
    unsigned char dst[IP_ADDRESS_LENGTH];
    unsigned char padding[IP_PADDING];
}IP;

int getIPLength(char *buf) {
    unsigned char a = buf[0];
    // printf("length: 0x%x\n", a);
    a = (char)((int) a % 0x10);
    int length = a * 4;
    IP_LENGTH = length;
    return length;
}

IP *getIP(char *buf) {
    IP *ip = (IP*)malloc(sizeof(IP));
    int ip_length = getIPLength(buf);
    memset(ip, 0, sizeof(IP));
    memcpy(ip, buf, ip_length);
    return ip;
}

char *getSrcIpAddress(IP *ip) {
    unsigned char *src = ip->src;
    char *srcIp = (char*)malloc(sizeof(char) * 0x10);
    memset(srcIp, 0, 0x10);
    int i = 0;
    sprintf(srcIp, "%d.%d.%d.%d", src[0], src[1], src[2], src[3]);
    return srcIp;
}

char *getDstIpAddress(IP *ip) {
    unsigned char *dst = ip->dst;
    char *dstIp = (char*)malloc(sizeof(char) * 0x10);
    memset(dstIp, 0, 0x10);
    sprintf(dstIp, "%d.%d.%d.%d", dst[0], dst[1], dst[2], dst[3]);
    // printf("%s", dstIp);
    return dstIp;
}

unsigned char *formatIp(char *ip) {
    unsigned char *ptr;
    int i = 0;
    unsigned char *ans = (unsigned char*)malloc(4);
    
    ptr = strtok(ip, ".");
    ans[i++] = atoi(ptr);
    while ((ptr = strtok(NULL, ".")) != NULL) {
        ans[i++] = atoi(ptr);
    }
    return ans;
}

void setSrcIpAddress(IP *ip, const char *src) {
    char *__src = (char*)malloc(sizeof(char) * IP_ADDRESS_LENGTH);
    strcpy(__src, src);
    unsigned char *formatSrc = formatIp(__src);
    memcpy(ip->src, formatSrc, sizeof(char) * IP_ADDRESS_LENGTH);
    free(__src);
}

void setDstIpAddress(IP *ip, const char *dst) {
    char *__dst = (char*)malloc(sizeof(char) * IP_ADDRESS_LENGTH);
    strcpy(__dst, dst);
    char *formatDst = formatIp(__dst);
    memcpy(ip->src, formatDst, sizeof(char) * IP_ADDRESS_LENGTH);
    free(__dst);
}


#endif // _IP_H_