/**
 * author: stdout
 * 
 **/
/* includes */
#include <pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "net/packetInformation.h"

/* defines */
#define MAX_PACKETS 15

/* 文佬说要面向对象 */
#define class struct

/* structs */
typedef struct {
    PACKET **packet;
    PacketInformation **pi;
    int number;
    int *length;
}PacketArray;

/* datas */
PacketArray packets;
pcap_t *handle;		        /* Session handle */
int file;


/* functions */
void succ_output(const char *buf) {
    printf("\x1b[32m[+] %s \x1b[0m \n", buf);
    printf("\n");
}

int appendPacket(PACKET *p, int length) {
    if (packets.number >= MAX_PACKETS) {
        return 0;
    }
    PacketInformation *pi = getPacketInformation(p);
    packets.pi[packets.number] = pi;
    packets.packet[packets.number] = p;
    packets.length[packets.number] = length;
    packets.number++;
    return 1;
}

void initPackets() {
    packets.number = 0;
    packets.packet = malloc(sizeof(PACKET*) * MAX_PACKETS);
    packets.length = malloc(sizeof(int) * MAX_PACKETS);
    packets.pi = malloc(sizeof(PacketInformation) * MAX_PACKETS);
}

void savePackets() {
    int f;
    f = open("./a.pcap", O_RDWR | O_CREAT);
    for (int i = 0; i < packets.number; i++) {
        write(f, packets.packet[i]->mac, 14);
        write(f, packets.packet[i]->ip, 20);
        if (packets.packet[i]->type == TCPPROTOCOL) {
            write(f, packets.packet[i]->propacket, TCP_LENGTH);
        } else {
            write(f, packets.packet[i]->propacket, ICMPPROTOCOL);
        }
        write(f, packets.packet[i]->data, DATA_LENGTH);

    }
    succ_output("write successfully!");
    close(f);
}

void savePacket(PACKET *p) {   
    int f;
    f = open("./packet.pcap", O_RDWR | O_CREAT);
    // for (int i = 0; i < packets.number; i++) {
        write(f, p->mac, MAC_LENGTH);
        write(f, p->ip, IP_LENGTH);
        if (p->type == TCPPROTOCOL) {
            write(f, p->propacket, TCP_LENGTH);
        } else {
            write(f, p->propacket, ICMPPROTOCOL);
        }
        write(f, p->data, DATA_LENGTH);

    // }
    succ_output("write successfully!");
    close(f);
}

void deletePackets() {
    for (int i = 0; i < packets.number; i++) {
        deletePacket(packets.packet[i]);
        packets.packet[i] = NULL;
        free(packets.pi[i]);
    }
    free(packets.packet);
    free(packets.pi);
    packets.packet = NULL;
}

void die(int status) {
    pcap_close(handle);
    char i = 0;
    printf("total packets: %d\n", packets.number);
    printf("do you want to save data?(y/n)\n");
    i = getchar();
    if (i == 'y') {
        savePackets();
    } 
    deletePackets();
}

void spoof(PACKET *p) {
    ICMP* icmp = (ICMP*)malloc(sizeof(ICMP));
    printf("%p\n", p->propacket);
    ICMP* tmps = p->propacket->icmp;
    printf("tmp: %p\n",tmps);
    memcpy(icmp, p->propacket->icmp, ICMP_LENGTH);
    icmp->type[0] = 0;
    icmp->code[0] = 0;
    icmp->checkSum[0] = 0;
    icmp->checkSum[1] = 0;

    unsigned char *data = p->data;

    char *tmp = (char*)malloc(sizeof(char) * 0x100);
    memset(tmp, 0, 0x100);
    memcpy(tmp, icmp, 0x10);
    memcpy(tmp + 0x10, data, 48);

    int sum = cal_chksum((unsigned short *)tmp, 64);
    icmp->checkSum[0] = sum % 0x100;
    icmp->checkSum[1] = sum / 0x100;


    IP *ip = (IP*)malloc(sizeof(IP));
    memset(ip, 0, sizeof(IP));
    ip->version_header_length[0] = 0x45;
    ip->ip_total_length[1] = 0x54;
    memcpy(ip->identification, p->ip->identification, IP_IDENTIFICATION_LENGTH);
    ip->ttl[0] = 0x12;
    ip->protocol[0] = 0x1;
    memcpy(ip->dst, p->ip->src, IP_ADDRESS_LENGTH);
    memcpy(ip->src, p->ip->dst, IP_ADDRESS_LENGTH);

    char buf[97] = { 0 };
    memcpy(buf, ip, 20);
    memcpy(buf + 20, icmp, 0x10);
    memcpy(buf + 36, data, 48);
    // IP *ips = getIP(buf);
    // printf("src: %s\n", getSrcIpAddress(ips));
    // exit(-1);

    // freebuffers
    free(tmp);
    free(ip);
    free(icmp);
    sendIcmp(getSrcIpAddress(p->ip), buf);
    // exit(-1);
    // ip->version_header_length
}

void got_packet(u_char *args, const struct pcap_pkthdr *header,
	    const u_char *packet) {
            // char c;
            PACKET *p = getPacket((char*)packet);
            if (p->type == ICMPPROTOCOL) {
            //    printf("\x1b[33m\x1b[4mdo you want to spoof it?(y/n)\x1b[0m\n");
            //    c = getchar();
            //    if (c == 'y') {
                    spoof(p);
            //    }
            }
            PacketInformation *pi = getPacketInformation(p);
            if (!appendPacket(p, header->len)) {
                fprintf(stderr, "packet is full\n");
                fprintf(stderr, "max: %d\n", MAX_PACKETS);
                die(-1);
                exit(-1);
            }
            showInformation(pi, packets.number);
            savePacket(p);
            
        }

void start() {
    char dev[] = "ens33";		/* Device to sniff on */
    char errbuf[PCAP_ERRBUF_SIZE];	/* Error string */
    struct bpf_program fp;		/* The compiled filter expression */
    char filter_exp[] = "icmp and src host 192.168.2.178";	/* The filter expression */
    bpf_u_int32 mask;		    /* The netmask of our sniffing device */
    bpf_u_int32 net;		    /* The IP of our sniffing device */

    struct pcap_pkthdr header;
    const u_char *packet;
    char *buf;

    if (pcap_lookupnet(dev, &net, &mask, errbuf) == -1) {
        fprintf(stderr, "Can't get netmask for device %s\n", dev);
        net = 0;
        mask = 0;
        exit(-1);
    }
    succ_output("get netmask successfully");
    handle = pcap_open_live(dev, BUFSIZ, 1, 1000, errbuf);
    if (handle == NULL) {
        fprintf(stderr, "Couldn't open device %s: %s\n", dev, errbuf);
        exit(-1);
    }
    succ_output("open device successfully");
    if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) {
        fprintf(stderr, "Couldn't parse filter %s: %s\n", filter_exp, pcap_geterr(handle));
        exit(-1);
    } 
    succ_output("parse filter successfully");
    if (pcap_setfilter(handle, &fp) == -1) {
        fprintf(stderr, "Couldn't install filter %s: %s\n", filter_exp, pcap_geterr(handle));
        exit(-1);
    }
    succ_output("install filter successfully");

    // packet = pcap_next(handle, &header);
    /* Print its length */

    for (int i = 0; i < 16; i++) {
        pcap_loop(handle, 1, got_packet, NULL);
        // buf = realloc(buf, 0x100);
    }
    
    /* And close the session */

}

int main() {
    initPackets();
    start();
    die(0);
    succ_output("successfully stoped!");
    return 0;
}