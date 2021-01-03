#include <linux/init.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netfilter_bridge.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/tcp.h>

const char *banIp = "192.168.2.191";
const char *vpsIp = "47.115.154.103";
 
static void
IP2Str(char *ipaddr, int size, uint32_t ip)
{
        snprintf(ipaddr, size, "%d.%d.%d.%d", ( ip >> 24 ) & 0xff
                                        , ( ip >> 16 ) & 0xff
                                        , ( ip >> 8 ) & 0xff
                                        , ip & 0xff);
}

int Intcmp(unsigned int a, unsigned int b) {
        return a == b;
} 

// ban dest ip from banip and vpsip
int banSrcIp(char *srcaddr) {
        return !(strcmp(srcaddr, banIp) && strcmp(srcaddr, vpsIp));
}

// ban dest ip to banip
int banDstIp(char *dstaddr) {
        return !(strcmp(dstaddr, banIp));
}

// ban dest port to 8848
int banDstPort(unsigned int dstPort) {
        return !(Intcmp(dstPort, 8848));
}

// ban source port to 8848
int banSrcPort(unsigned int srcPort) {
        return !(Intcmp(srcPort, 8848));
}

int gotIcmpPacket(struct iphdr *iph) {
        char srcaddr[17];
        char dstaddr[17];

        memset(srcaddr, 0, sizeof(srcaddr));
        memset(dstaddr, 0, sizeof(dstaddr));

        IP2Str(srcaddr, sizeof(srcaddr), ntohl(iph->saddr));
        IP2Str(dstaddr, sizeof(dstaddr), ntohl(iph->daddr));
        if (banSrcIp(srcaddr) || banDstIp(dstaddr)) {
                printk(KERN_DEBUG "drop a packet from %s to %s", srcaddr, dstaddr);
                return 1;
        }
}

int gotTcpPacket(struct iphdr *iph, struct tcphdr *tcph) {
        char srcaddr[17];
        char dstaddr[17];
        unsigned int srcPort = 0;
        unsigned int dstPort = 0;

        memset(srcaddr, 0, sizeof(srcaddr));
        memset(dstaddr, 0, sizeof(dstaddr));

        IP2Str(srcaddr, sizeof(srcaddr), ntohl(iph->saddr));
        IP2Str(dstaddr, sizeof(dstaddr), ntohl(iph->daddr));
        dstPort = ntohl(tcph->dest);
        srcPort = ntohl(tcph->source);

        if (banSrcIp(srcaddr) || banDstIp(dstaddr) || banDstPort(dstPort) || banSrcPort(srcPort)) {
                printk(KERN_DEBUG "drop a packet from %s:%hu to %s:%hu", srcaddr, srcPort, dstaddr, dstPort);
                return 1;
        }
}

unsigned int
my_hook_fun(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{
        struct iphdr *iph; 
        struct tcphdr *tcph;

        if( unlikely(!skb) ) {
                return NF_ACCEPT;
        }
 
        iph = ip_hdr(skb);
        if( unlikely(!iph) ) {
                return NF_ACCEPT;
        }
 
        if( likely(iph->protocol == IPPROTO_ICMP) ) {
                // got the packet
                if (gotIcmpPacket(iph)) {
                        return NF_DROP;
                }
        } else if (likely(iph->protocol == IPPROTO_TCP)) {
                tcph = tcp_hdr(skb);
                if (gotTcpPacket(iph, tcph)) {
                        return NF_DROP;
                }
        } 
 
        return NF_ACCEPT;
}
 
static struct nf_hook_ops my_hook_ops = {
        .hook           = my_hook_fun,          //hook处理函数
        .pf             = PF_INET,              //协议类型
        .hooknum        = NF_BR_PRE_ROUTING,    //hook注册点
        .priority       = NF_IP_PRI_FIRST,      //优先级->top
};
 
static void
hello_cleanup(void) {
        nf_unregister_hook(&my_hook_ops);
}
 
static __init int hello_init(void) {
 
        if ( nf_register_hook(&my_hook_ops) != 0 ) {
                printk(KERN_WARNING "register hook error!\n");
                goto err;
        }
        printk(KERN_ALERT "init hook successfully!\n");
        return 0;
 
err:
        hello_cleanup();
        return -1;
}
 
static __exit void hello_exit(void) {
        hello_cleanup();
        printk(KERN_WARNING "exit hook successfully!\n");
}
 
module_init(hello_init);
module_exit(hello_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Stone");