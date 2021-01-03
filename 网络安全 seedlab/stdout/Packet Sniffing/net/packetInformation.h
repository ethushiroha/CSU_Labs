#ifndef _PACKET_INFORMATION_H_
#define _PACKET_INFORMATION_H_

#include "packet.h"

typedef struct { // PacketInformation
    int type; // 0 -> tcp 1 -> udp  2 -> icmp
    char *dstIp;
    char *srcIp;
    char *dstMac;
    char *srcMac;
    char *dstPort;
    char *srcPort;
    char *protocol;
    char *data;
}PacketInformation;


PacketInformation *getPacketInformation(PACKET *p) {
    PacketInformation *pi = (PacketInformation*)malloc(sizeof(PacketInformation));
    pi->dstIp = getDstIpAddress(p->ip);
    pi->srcIp = getSrcIpAddress(p->ip);
    pi->dstMac = getDstMacAddress(p->mac);
    pi->srcMac = getSrcMacAddress(p->mac);
    pi->type = p->type;

    switch (pi->type)
    {
    case TCPPROTOCOL:
        pi->protocol = "tcp";
        pi->dstPort = getDstPort(p->propacket);
        pi->srcPort = getSrcPort(p->propacket);
        break;
    case ICMPPROTOCOL:
        pi->protocol = "icmp";
        pi->dstPort = "\x1b[32mICMP has no port\x1b[0m";
        pi->srcPort = "\x1b[32mICMP has no port\x1b[0m";
        break;
    default:
        pi->protocol = NULL;
        break;
    }
    pi->data = p->data;
    return pi;
}

void showInformation(PacketInformation *pi, int number) {
    printf("+========================+\n");
    printf("recv(%d)\n", number);
    printf("protocol: \x1b[32m %s\n\x1b[0m", pi->protocol);
    printf("from: \n");
    printf("\tsrc mac: %s\n", pi->srcMac);
    printf("\tsrc ip: %s\n", pi->srcIp);
    printf("\tsrc port: %s\n", pi->srcPort);
    printf("to: \n");
    printf("\tdst mac: %s\n", pi->dstMac);
    printf("\tdst ip: %s\n", pi->dstIp);
    printf("\tdst port: %s\n", pi->dstPort);
    if (pi->type != ICMPPROTOCOL) {
        printf("data: \n");
        printf("\tdata: %s\n", pi->data);
    }
    printf("+========================+\n");
}


#endif // _PACKETINFORMATION_H_