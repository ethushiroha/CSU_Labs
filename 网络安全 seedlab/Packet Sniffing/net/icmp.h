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

    if(sendto(sockfd, icmp, 0x62 - 14, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0) {
        perror("sendto error");
        return 0;
    }
    printf("send to: %s, send done\n", ip);
    close(sockfd);
    return 1;

}


#endif // _ICMP_H_