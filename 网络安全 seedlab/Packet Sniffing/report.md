# Packet Sniffing and Spoofing Lab

## Task 2.1: Writing Packet Sniffing Program

[pcap.h Reference Link](http://www.tcpdump.org/pcap.htm)

### Task 2.1A: Understanding How a Sniffer Works

以太网包结构示意图如下：

![20181128203128262](https://i.loli.net/2020/10/09/MLdFRqKx6G8TXtj.png)

#### net

##### mac.h

用于封装mac层的数据

```c
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
```



##### ip.h

用于封装ip层的数据

```c
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
```



##### icmp.h

用于封装icmp的数据

```c
#ifndef _ICMP_H_
#define _ICMP_H_

#include "ip.h"
#include <stdio.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <setjmp.h>
#include <errno.h>
#include <string.h>
#include <pcap.h>

#define ICMP_TYPE                   1
#define ICMP_CODE                   1
#define ICMP_CHECKSUM               2
#define ICMP_ID                     2
#define ICMP_SEQUENCE               2
#define ICMP_TIMESTAMP              8

int ICMP_LENGTH = 0x10;

typedef struct { // ICMP
    unsigned char type[ICMP_TYPE];
    unsigned char code[ICMP_CODE];
    unsigned char checkSum[ICMP_CHECKSUM];
    unsigned char id[ICMP_ID];
    unsigned char sequence[ICMP_SEQUENCE];
    unsigned char TimeStamp[ICMP_TIMESTAMP];
}ICMP;

int getIcmpLength() {
    ICMP_LENGTH = 0x10;
    return 0x10;
}

ICMP *getIcmp(char *buf) {
    ICMP *icmp = (ICMP*)malloc(sizeof(ICMP) + 1);
    int length = getIcmpLength();
    memset(icmp, 0, ICMP_LENGTH);
    memcpy(icmp, buf, ICMP_LENGTH);
}

/* 计算校验和的算法 */
unsigned short cal_chksum(unsigned short *addr, int len)  {
    int sum=0;
    int nleft = len;
    unsigned short *w = addr;
    unsigned short answer = 0;
    /* 把ICMP报头二进制数据以2字节为单位累加起来 */
    while(nleft > 1){
        sum += *w++;
        nleft -= 2;
    }
    /*
     * 若ICMP报头为奇数个字节，会剩下最后一字节。
     * 把最后一个字节视为一个2字节数据的高字节，
     * 这2字节数据的低字节为0，继续累加
     */
    if(nleft == 1){
        *(unsigned char *)(&answer) = *(unsigned char *)w;
        sum += answer;    /* 这里将 answer 转换成 int 整数 */
    }
    sum = (sum >> 16) + (sum & 0xffff);        /* 高位低位相加 */
    sum += (sum >> 16);        /* 上一步溢出时，将溢出位也加到sum中 */
    answer = ~sum;             /* 注意类型转换，现在的校验和为16位 */
    return answer;
}

int sendIcmp(char *ip, char *icmp) {
    struct protoent *protocol;
    protocol = getprotobyname("ip");
    int sockfd;
    int size = 50*1024;

    struct sockaddr_in dest_addr;
    bzero(&dest_addr, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = inet_addr(ip);
    
    if((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) {
        perror("socket error");
    }
    // setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size) );

    if(sendto(sockfd, icmp, 0x62 - MAC_LENGTH, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0) {
        perror("sendto error");
        return 0;
    }
    printf("send to: %s, send done\n", ip);
    close(sockfd);
    return 1;

}


#endif // _ICMP_H_
```



##### tcp.h

用于封装tcp的包

```c
#ifndef _TCP_H_
#define _TCP_H_

#include "ip.h"

// defines
    // tcp
#define TCP_PORT_LENGTH             2
#define TCP_ORDER_ACCEPT            8
        // other headers
#define TCP_FUCK                    20

int TCP_LENGTH = 0;

typedef struct { // TCP
    // IP ip_header;
    unsigned char src_port[TCP_PORT_LENGTH];
    unsigned char dst_port[TCP_PORT_LENGTH];
    unsigned char order_accept[TCP_ORDER_ACCEPT];
    unsigned char fuck[TCP_FUCK];
}TCP;

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
```



##### Packet.h

用于封装整个包

```c
#ifndef _PACKET_H_
#define _PACKET_H_

#include "tcp.h"
#include "icmp.h"

typedef struct { // PACKET
    MAC *mac;
    IP *ip;
    union {
        TCP *tcp;
        UDP *udp;
        ICMP *icmp;
    } *propacket;
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
    PACKET *p = (PACKET*)malloc(sizeof(PACKET));

    MAC *mac = getMac(buf);
    p->mac = mac;
    buf += MAC_LENGTH;

    IP *ip = getIP(buf);
    p->ip = ip;
    buf += IP_LENGTH;

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
        p->data = data;
        p->type = ICMPPROTOCOL;
        break;
        
    case 6: // tcp packet
        printf("\x1b[33m\x1b[4mgot a tcp packet\x1b[0m\n");
        tcp = getTCP(buf);
        p->propacket = tcp;
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
    MAC_LENGTH = 14;
    IP_LENGTH = 0;
    TCP_LENGTH = 0;
    ICMP_LENGTH = 0;
    DATA_LENGTH = 0;
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
```



**main.c (capture tcp packets [filter rules: tcp and host 47.115.154.103] )**

```c
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
        write(f, p->mac, MAC_LENGTH);
        write(f, p->ip, IP_LENGTH);
        if (p->type == TCPPROTOCOL) {
            write(f, p->propacket, TCP_LENGTH);
        } else {
            write(f, p->propacket, ICMPPROTOCOL);
        }
        write(f, p->data, DATA_LENGTH);

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
    ip->ttl[0] = 0x1;
    ip->protocol[0] = 0x1;
    memcpy(ip->dst, p->ip->src, IP_ADDRESS_LENGTH);
    memcpy(ip->src, p->ip->dst, IP_ADDRESS_LENGTH);

    char buf[97] = { 0 };
    memcpy(buf, ip, 20);
    memcpy(buf + 20, icmp, 0x10);
    memcpy(buf + 36, data, 48);

    free(tmp);
    free(ip);
    free(icmp);
    sendIcmp(getSrcIpAddress(p->ip), buf);
}

void got_packet(u_char *args, const struct pcap_pkthdr *header,
	    const u_char *packet) {
            // char c;
            PACKET *p = getPacket((char*)packet);
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
    char filter_exp[] = "tcp and host 47.115.154.103";	/* The filter expression */
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

    for (int i = 0; i < 16; i++) {
        pcap_loop(handle, 1, got_packet, NULL);
    }
}

int main() {
    initPackets();
    start();
    die(0);
    succ_output("successfully stoped!");
    return 0;
}
```

让**`ubuntu`与`47.115.154.103`主机的8848端口发生tcp通信**，并成功监听到双方发送的TCP数据包

![image-20201018135254342](https://gitee.com/ethustdout/pics/raw/master/img/image-20201018135254342.png)

利用hexedit查看抓包dump下来的Capture.out文件：

![image-20201018142828557](https://gitee.com/ethustdout/pics/raw/master/img/image-20201018142828557.png)

* Question 1. 请总结描述对Sniffer必要库调用的顺序

  > 1. 查找可监听的有效网卡设备(ifconfig/pcap_lookupdev)
  >
  > 2. 打开监听设备，pcap_t *pcap_open_live(char *device, int snaplen, int promisc, int to_ms, char *errbuf);
  >
  > 3. 设置过滤条件(3 steps)
  >
  >      1) 查找当前ip及子网掩码，int pcap_lookupnet(const char *device, bpf_u_int32 *net, bpf_u_int32 *mask, char *errbuf);
  >
  >      2) 编译过滤条件，int pcap_compile(pcap_t *p, struct bpf_program *fp, char *str, int optimize, bpf_u_int32 netmask);
  >
  >      3) 设置过滤条件，int pcap_setfilter(pcap_t *p, struct bpf_program *fp);
  >
  > 4. 循环抓包(pcap_loop)
  >
  >      int pcap_loop(pcap_t *p, int cnt, pcap_handler callback, u_char *user);
  >
  >      对数据包定义回调函数：pkt_callback(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
  >
  >      ```c
  >      struct pcap_pkthdr {
  >      		struct timeval ts; /* time stamp */
  >      		bpf_u_int32 caplen; /* length of portion present */
  >      		bpf_u_int32 len; /* length this packet (off wire) */
  >      	};
  >      ```

* Question 2. 为什么需要root权限来运行Sniffer？如果没有root权限，则程序将在哪里失败？

  > Sniffer需要监听网卡，但经过网卡的流量只允许用户以root权限监听
  >
  > 如果以非root权限运行Sniffer，会在pcap_lookupdev时就提示权限不足
  >
  > 附：tcpdump工具同样也需要root权限监听，但可由root人为设置其他用户具有使用tcpdump监听网卡的权限

  ![image-20201018144248559](https://gitee.com/ethustdout/pics/raw/master/img/image-20201018144248559.png)

* Question 3. 请打开或关闭Sniffer的混淆模式，并演示有何区别

  > 在混淆模式下，网络中发送的每个帧都将通过网卡传递给内核，无论它们是否寻址到内核
  >
  > pcap_open_live的promisc参数指定是否打开混淆模式。关闭混淆模式，则只抓取进入本机或者在本机路由转发的包， 而打开混淆模式会抓取网络上所有的包。
  >
  > **混淆模式下在192.168.2.186上抓取到192.168.2.178->47.115.154.103的TCP数据包**
  >
  > ![image-20201018144958091](https://gitee.com/ethustdout/pics/raw/master/img/image-20201018144958091.png)
  >
  > **非混淆模式下在192.168.2.186上已抓取不到192.168.2.178->47.115.154.103的TCP数据包**
  >
  > ![image-20201018145253776](https://gitee.com/ethustdout/pics/raw/master/img/image-20201018145253776.png)

### Task 2.1B: Writing Filters

* 抓取两个指定IP间的ICMP数据包

  ```c
  char filter_exp[] = "icmp and ((src host 192.168.2.178 and dst host 192.168.2.186) or (src host 192.168.2.186 and dst host 192.168.2.178))";
  ```

  ![image-20201018145616938](https://gitee.com/ethustdout/pics/raw/master/img/image-20201018145616938.png)

* 抓取指定目的ip，端口10~100的TCP数据包

  ```c
  char filter_exp[] = "tcp and dst host 47.115.154.103 and dst portrange 1000-10000";
  ```

  ![image-20201018150123534](https://gitee.com/ethustdout/pics/raw/master/img/image-20201018150123534.png)

### Task 2.1C: Sniffing Passwords

telnet client: 192.168.2.178

telnet server: 192.168.2.186

password: **12***(逐字节传输)

![image-20201018151119212](https://gitee.com/ethustdout/pics/raw/master/img/image-20201018151119212.png)

##  Task 2.2: Spoofing

### Task 2.2A: Write a spoofing program

采用原始套接字编程实现构造IP层及以上的数据包任意字段（`SOCK_RAW`）

需在`setsockopt`时开启`IP_HDRINCL`，才能自行构造`IP_header

[源代码都在我的github上](https://github.com/ethushiroha/seedlab/tree/master/Packet%20Sniffing)

```c
#include <stdio.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <setjmp.h>
#include <errno.h>
#include <string.h>
#include <pcap.h>
#include "net/icmp.h"


#define PACKET_SIZE 4096

void spoof(char *ips) {
    ICMP* icmp = (ICMP*)malloc(sizeof(ICMP));
    icmp->type[0] = 8;
    icmp->code[0] = 0;
    icmp->checkSum[0] = 0;
    icmp->checkSum[1] = 0;

    unsigned char *data = (unsigned char*)malloc(sizeof(unsigned char) * 48);
    memset(data, 0, 48);
    memcpy(data, "stdout do not like C language, do you think so?", 48);

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
    ip->identification[0] = 0;
    ip->identification[1] = 0;
    // memset(ip->identification, 0, IP_IDENTIFICATION_LENGTH);
    ip->ttl[0] = 0x1;
    ip->protocol[0] = 0x1;
    ip->dst[0] = 0xc0;
    ip->dst[1] = 0xa8;
    ip->dst[2] = 0x2;
    ip->dst[3] = 0xb2;
    ip->src[0] = 0x2f;
    ip->src[1] = 0x73;
    ip->src[2] = 0x9a;
    ip->src[3] = 0x67;
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
    sendIcmp("192.168.2.178", buf);
    // exit(-1);
    // ip->version_header_length
}

int main() {
    char *ip = "192.168.2.178";
    spoof(ip);
}
```

```sh
gcc test.c -o test -w
```



### Task 2.2B: Spoof an ICMP Echo Request

通过原始套接字编程实现任意源IP的ping echo-request伪造(ICMP)

可以看到，Spoofer已成功伪造源ip为`127.0.0.1`的ping echo-Request，并且返回了一个reply

![image-20201018154328384](https://gitee.com/ethustdout/pics/raw/master/img/image-20201018154328384.png)



* Question 4. 无论实际数据包有多大，是否可以将IP数据包长度字段设置为任意值？

  > ip_total_len字段设置过小时，socket在封装数据链路层等数据后，发包  时会提示报错
  >
  > `Packet are not sent : Message too long`
  >
  > 而在该字段设置过大时，ip数据包会生成出错，导致被truncate
  >
  > 因此ip_total_len字段必须是正确的ip_header与tcp/udp/icmp长度之和
  
* Question 5. 使用原始套接字编程时，是否必需自己计算ip头的校验和字段？

  > 不必要，ip头的校验和计算会在发包至网络前由os自动计算，所以不同于TCP等的校验和设置，即使IP头的校验和字段设置错误，仍能被os重新计算校验和，实现自动纠正

* Question 6. 为什么需要root权限来运行Sniffer？如果没有root权限，则程序将在哪里失败？

  > 与pcap.h一样，套接字发包的过程也需要操作网卡，在root权限下，我们可以借助原始套接字编程来修改包里的任意字段并进行发包，反之不能
  >
  > 且实验发现虽然运行./spoofing不报错，但发包失败（wireshark未监听到预期流量）

### Task 2.3: Sniff and then Spoof

在主机B上嗅探主机A发送的任意ping(echo-request)包，并无论主机A所ping地址主机是否存活，均伪造echo-reply包，对主机A实现欺骗

对sniffer和spoofer作部分修改即可

**main.c**

```c
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
    ip->ttl[0] = 0x1;
    ip->protocol[0] = 0x1;
    memcpy(ip->dst, p->ip->src, IP_ADDRESS_LENGTH);
    memcpy(ip->src, p->ip->dst, IP_ADDRESS_LENGTH);

    char buf[97] = { 0 };
    memcpy(buf, ip, 20);
    memcpy(buf + 20, icmp, 0x10);
    memcpy(buf + 36, data, 48);
    
    free(tmp);
    free(ip);
    free(icmp);
    sendIcmp(getSrcIpAddress(p->ip), buf);
}

void got_packet(u_char *args, const struct pcap_pkthdr *header,
	    const u_char *packet) {
            // char c;
            PACKET *p = getPacket((char*)packet);
            if (p->type == ICMPPROTOCOL) {
                    spoof(p);
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
    char filter_exp[] = "tcp and src host 192.168.2.178";	/* The filter expression */
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

    for (int i = 0; i < 16; i++) {
        pcap_loop(handle, 1, got_packet, NULL);
    }
}

int main() {
    initPackets();
    start();
    die(0);
    succ_output("successfully stoped!");
    return 0;
}
```

需注意的是ICMP在配对ping echo-request和ping echo-reply时，需要**比对icmp_id, icmp_seq，及icmp报文数据（时间戳等认证信息）**，否则会被认为是无效reply包

所以选择直接memcpy原icmp段，再进行ip头src_ip, dst_ip，icmp头icmp_type, icmp_code的修改，以及校验和重计算

![image-20201018162247172](https://gitee.com/ethustdout/pics/raw/master/img/image-20201018162247172.png)

