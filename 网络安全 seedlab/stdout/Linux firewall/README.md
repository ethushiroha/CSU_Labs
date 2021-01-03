# Linux Firewall Exploration Lab



## 2 Lab Tasks

`seedubuntu1`

```shell
2: ens33: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast state UNKNOWN group default qlen 1000
    link/ether 00:0c:29:7a:5d:47 brd ff:ff:ff:ff:ff:ff
    inet 192.168.2.189/24 brd 192.168.2.255 scope global dynamic ens33
       valid_lft 1767sec preferred_lft 1767sec
    inet6 fe80::9688:b76f:c987:76b1/64 scope link 
       valid_lft forever preferred_lft forever
```

`seedubuntu2`

```shell
2: ens33: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast state UNKNOWN group default qlen 1000
    link/ether 00:0c:29:d9:49:cc brd ff:ff:ff:ff:ff:ff
    inet 192.168.2.191/24 brd 192.168.2.255 scope global dynamic ens33
       valid_lft 1415sec preferred_lft 1415sec
    inet6 fe80::f582:3390:83a2:3e20/64 scope link 
       valid_lft forever preferred_lft forever
```



### Task 1: Using Firewall

- 防止A使用`telnet`连接到B

	正常情况下，可以连通

	![image-20201023171651064](https://gitee.com/ethustdout/pics/raw/master/img/image-20201023171651064.png)

	在B终端输入`iptables -A INPUT -s 192.168.2.189 -j DROP`

	再次使用`telnet`，连接失败

	![image-20201023172009623](https://gitee.com/ethustdout/pics/raw/master/img/image-20201023172009623.png)

- 防止B使用`telnet`连接到A

	同理，在A中输入相同的指令即可
	
- 防止A连接到任意web站点（这里使用我的服务器`47.115.154.103`）

	正常情况下，可以ping通

	![image-20201025161239667](https://gitee.com/ethustdout/pics/raw/master/img/image-20201025161239667.png)

	使用`iptables -A OUTPUT -d 47.115.154.103 -j DROP`

	![image-20201025161351409](https://gitee.com/ethustdout/pics/raw/master/img/image-20201025161351409.png)



### Task 2: Implementing a Simple Firewall

如下代码，阻止来自`banIp vpsIp`和`8848端口`的所有数据包

```c
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
```

正常情况下，可以互相`ping`通（`icmp`协议）

![image-20201026143446280](https://gitee.com/ethustdout/pics/raw/master/img/image-20201026143446280.png)

加载模块之后，不互通，查看内核输出，丢弃了数据包

![image-20201026143538494](https://gitee.com/ethustdout/pics/raw/master/img/image-20201026143538494.png)

`tcp`同理，使用`nc`互相连接，但是在加载了模块的情况下，无法建立连接



###  Task 3: Evading Egress Filtering

重新配置`ip`

`seedubuntu1(gw)`

```shell
2: ens33: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast state UNKNOWN group default qlen 1000
    link/ether 00:0c:29:7a:5d:47 brd ff:ff:ff:ff:ff:ff
    inet 192.168.2.189/24 brd 192.168.2.255 scope global dynamic ens33
       valid_lft 1144sec preferred_lft 1144sec
    inet6 fe80::9688:b76f:c987:76b1/64 scope link 
       valid_lft forever preferred_lft forever
3: ens38: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast state UP group default qlen 1000
    link/ether 00:0c:29:7a:5d:51 brd ff:ff:ff:ff:ff:ff
    inet 192.168.52.135/24 brd 192.168.52.255 scope global dynamic ens38
       valid_lft 941sec preferred_lft 941sec
    inet6 fe80::59a:db57:5c6d:49d6/64 scope link 
       valid_lft forever preferred_lft forever
```

`seedubuntu2(nat)`

```shell
2: ens33: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast state UP group default qlen 1000
    link/ether 00:0c:29:d9:49:cc brd ff:ff:ff:ff:ff:ff
    inet 192.168.52.134/24 brd 192.168.52.255 scope global dynamic ens33
       valid_lft 1029sec preferred_lft 1029sec
    inet6 fe80::f582:3390:83a2:3e20/64 scope link 
       valid_lft forever preferred_lft forever
```

`ubuntu(service)`

```shell
2: ens33: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc fq_codel state UP group default qlen 1000
    link/ether 00:0c:29:5e:18:b0 brd ff:ff:ff:ff:ff:ff
    inet 192.168.2.186/24 brd 192.168.2.255 scope global dynamic noprefixroute ens33
       valid_lft 938sec preferred_lft 938sec
    inet6 fe80::b407:283e:8d99:df25/64 scope link noprefixroute 
       valid_lft forever preferred_lft forever
```

配置转发，使`seedubuntu2`能够通过`seedubuntu1`连接到外部网络



#### Task 3.a: Telnet to Machine B through the firewall

在`seedubuntu2`上使用`SSH -L 8848:192.168.2.186:23 -fN seed@192.168.52.135` 

![image-20201026154011148](https://gitee.com/ethustdout/pics/raw/master/img/image-20201026154011148.png)

成功连接到`ubuntu`



#### Task 3.b: Connect to Facebook using SSH Tunnel. 

`ssh -L 8848:47.115.154.103:8848 -fN seed@192.168.52.135`

打开浏览器，访问`http://127.0.0.1:8848`

![image-20201027085859619](https://gitee.com/ethustdout/pics/raw/master/img/image-20201027085859619.png)







###  Task 4: Evading Ingress Filtering

使用`反向ssh代理`

在内网机上先创建`index.php`

```php+HTML
<h1> this is a website </h1>
```

![image-20201110115237778](https://gitee.com/ethustdout/pics/raw/master/img/image-20201110115237778.png)

在内网机上使用`ssh -fNR 8848:127.0.0.1:80 seed@192.168.52.135`将本地的`80端口`转发到`gw`的`8848端口`

![image-20201110115515885](https://gitee.com/ethustdout/pics/raw/master/img/image-20201110115515885.png)

然后在`gw`上使用`ssh`本地端口转发

`ssh -fNCL *:1234:localhost:8848 localhost`

![image-20201110120436295](https://gitee.com/ethustdout/pics/raw/master/img/image-20201110120436295.png)

然后建立的连接就是

`192.168.52.135:80 -> 192.1168.52.134:8848 -> 192.168.52.134:1234`

所以访问`192.168.52.134:1234`就是访问内网的`web服务`

在`ubuntu`上访问`http://192.168.2.189:1234/index.php`

![image-20201110120749554](https://gitee.com/ethustdout/pics/raw/master/img/image-20201110120749554.png)

成功访问到了内网的web服务

`ssh`同理