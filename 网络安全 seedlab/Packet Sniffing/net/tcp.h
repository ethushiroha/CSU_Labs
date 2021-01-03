#ifndef _TCP_H_
#define _TCP_H_

#include "ip.h"

// defines
    // tcp
#define TCP_PORT_LENGTH             2
#define TCP_ORDER_ACCEPT            8
        // other headers
#define TCP_FUCK                    20
    //udp
#define UDP_PORT_LENGTH             2
#define UDP_PACKET_LENGTH           2
#define UDP_SUM                     2
        // padding
#define UDP_FUCK                    24



int TCP_LENGTH = 0;

typedef struct { // TCP
    // IP ip_header;
    unsigned char src_port[TCP_PORT_LENGTH];
    unsigned char dst_port[TCP_PORT_LENGTH];
    unsigned char order_accept[TCP_ORDER_ACCEPT];
    unsigned char fuck[TCP_FUCK];
}TCP;

typedef struct { // UDP
    // IP ip_header;
    unsigned char src_port[UDP_PORT_LENGTH];
    unsigned char dst_port[UDP_PORT_LENGTH];
    unsigned char packet_length[UDP_PACKET_LENGTH];
    unsigned char sum[UDP_SUM];
    unsigned char funk[UDP_FUCK];
}UDP;

int getTcpLength(char *buf) {
    unsigned char a = buf[12];
    a = (char)((int)a / 0x10);
    int length = a * 4;
    TCP_LENGTH = length;
    return length;
}

TCP *getTCP(char *buf) {
    TCP *tcp = (TCP*)malloc(sizeof(TCP) + 1);
    if (tcp == NULL) {
        fprintf(stderr, "malloc error\n");
        exit(-1);
    }
    int tcp_length = getTcpLength(buf);
    memset(tcp, 0, sizeof(TCP));
    memcpy(tcp, buf, tcp_length);
    return tcp;
}

char *getDstPort(TCP *tcp) {
    unsigned char *dst = tcp->dst_port;
    char *dstPort = (char*)malloc(sizeof(char) * 0x18);
    memset(dstPort, 0, 0x18);
    sprintf(dstPort, "%d", dst[0] * 0x100 + dst[1]);
    // printf("port: %s\n", dstPort);
    return dstPort;
}

char *getSrcPort(TCP *tcp) {
    unsigned char *src = tcp->src_port;
    char *srcPort = (char*)malloc(sizeof(char) * 0x8);
    memset(srcPort, 0, 0x8);
    sprintf(srcPort, "%d", src[0] * 0x100 + src[1]);
    // printf("port: %s\n", srcPort);
    return srcPort;
}



#endif // _TCP_H_