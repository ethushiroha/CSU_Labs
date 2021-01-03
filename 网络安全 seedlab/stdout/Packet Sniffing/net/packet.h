#ifndef _PACKET_H_
#define _PACKET_H_

#include "tcp.h"
#include "icmp.h"

typedef union {
    TCP *tcp;
    ICMP *icmp;
} propackets;

typedef struct { // PACKET
    MAC *mac;
    IP *ip;
    propackets *propacket;
    unsigned char *data;
    int type; // 0 -> tcp 1->udp
}PACKET;

unsigned char *getData(char *buf, IP *ip, int type) {
    unsigned int length = getDataLength(ip, type);
    unsigned char *data = (unsigned char*)malloc(length + 0x10);
    memset(data, 0, length + 0x10);
    memcpy(data, buf, length);
    return data;
}

int getPacketLength(IP *ip) {
    // printf("size: %d\n", sizeof(IP));
    int length = ip->ip_total_length[0] * 0x10 + ip->ip_total_length[1];
    length += 0xe;
    PACKET_LENGTH = length;
    return length;
}

PACKET *getPacket(char *buf) {

    MAC_LENGTH = 14;
    IP_LENGTH = 0;
    TCP_LENGTH = 0;
    ICMP_LENGTH = 0;
    DATA_LENGTH = 0;
    PACKET *p = (PACKET*)malloc(sizeof(PACKET));

    MAC *mac = getMac(buf);
    p->mac = mac;
    buf += MAC_LENGTH;

    IP *ip = getIP(buf);
    p->ip = ip;
    buf += IP_LENGTH;

    p->propacket = (propackets*)malloc(sizeof(propackets));
    memset(p->propacket, 0, sizeof(propackets));

    PACKET_LENGTH = getPacketLength(ip);

    TCP *tcp;
    UDP *udp;
    ICMP *icmp;
    unsigned char *data;
    switch ((int)ip->protocol[0]) {
    case 1: // icmp pakcet
        printf("\x1b[33m\x1b[4mgot an icmp packet\x1b[0m\n");
        icmp = getIcmp(buf);
        buf += ICMP_LENGTH;
        data = getData(buf, ip, ICMPPROTOCOL);
        buf += DATA_LENGTH;
        printf("1\n");
        p->propacket->icmp = icmp;
        printf("1\n");
        p->data = data;
        p->type = ICMPPROTOCOL;
        break;
        
    case 6: // tcp packet
        printf("\x1b[33m\x1b[4mgot a tcp packet\x1b[0m\n");
        tcp = getTCP(buf);
        p->propacket->tcp = tcp;
        buf += TCP_LENGTH;
        data = getData(buf, ip, TCPPROTOCOL);
        buf += DATA_LENGTH;
        p->data = data;
        p->type = TCPPROTOCOL;
        break;
    case 14: // udp packet

        break;
    default:
        break;
    }    
    return p;
}

int deletePacket(PACKET *p) {
    if (p != NULL) {
        free(p->mac);
        p->mac = NULL;
        free(p->ip);
        p->ip = NULL;
        free(p->propacket);
        p->propacket = NULL;
        free(p->data);
        p->data = NULL;
        return 1;
    }
    return 0;
}

int getDataLength(IP *ip, int type) {
    int length = getPacketLength(ip);
    switch (type)
    {
    case TCPPROTOCOL:
        length = length - MAC_LENGTH - IP_LENGTH - TCP_LENGTH;
        break;
    case ICMPPROTOCOL:
        length = length - MAC_LENGTH - IP_LENGTH - ICMP_LENGTH;
        break;

    default:
        length = 0;
        break;
    }
    DATA_LENGTH = length;
    return length;
}

#endif // _PACKET_H_