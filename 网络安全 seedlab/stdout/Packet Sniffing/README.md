#   Packet Sniffing



## 监听网卡

通过`pcap_lookupnet`函数获得指定网络设备的网络号和掩码

```c
char dev[] = "ens33";		/* Device to sniff on */
char errbuf[PCAP_ERRBUF_SIZE];	/* Error string */
struct pcap_pkthdr header;
const u_char *packet;

if (pcap_lookupnet(dev, &net, &mask, errbuf) == -1) {
    fprintf(stderr, "Can't get netmask for device %s\n", dev);
    net = 0;
    mask = 0;
    exit(-1);
}
```



通过`pcap_open_live`函数打开设备，监听网卡

```c
handle = pcap_open_live(dev, BUFSIZ, 1, 1000, errbuf);
if (handle == NULL) {
    fprintf(stderr, "Couldn't open device %s: %s\n", dev, errbuf);
    exit(-1);
}
```

此时网卡就开始监听了

![image-20201017200926209](https://gitee.com/ethustdout/pics/raw/master/img/image-20201017200926209.png)



## 设置过滤规则

使用`pcap_compile`和`pcap_setfilter`函数设置过滤器，这里将其设置为只抓取`icmp`类型的包

```c
char filter_exp[] = "icmp";	/* The filter expression */
if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) {
    fprintf(stderr, "Couldn't parse filter %s: %s\n", filter_exp, pcap_geterr(handle));
    exit(-1);
} 
succ_output("parse filter successfully");
if (pcap_setfilter(handle, &fp) == -1) {
    fprintf(stderr, "Couldn't install filter %s: %s\n", filter_exp, pcap_geterr(handle));
    exit(-1);
}
```

这样过滤器就设置好了。

![image-20201017200946514](https://gitee.com/ethustdout/pics/raw/master/img/image-20201017200946514.png)



## 抓包

使用`pcap_lookup`和一个回调函数`got_packet`抓取并对抓取到的流量包进行操作



```c
for (int i = 0; i < 16; i++) {	// 16 为抓取的数量
    pcap_loop(handle, 1, got_packet, NULL);
    // buf = realloc(buf, 0x100);
}
```

**`got_packet`**对包进行处理

```c
void got_packet(u_char *args, const struct pcap_pkthdr *header,
	    const u_char *packet) {
            printf("length: %d\n", header->len);
            
            PACKET *p = getPacket((char*)packet);
            spoof(p);
            if (!appendPacket(p, header->len)) {
                fprintf(stderr, "packet is full\n");
                fprintf(stderr, "max: %d\n", MAX_PACKETS);
                die(-1);
                exit(-1);
            }

            showInformation(packets.pi[packets.number - 1], packets.number);
        }
```

先把抓到的数据转换成包的形式，方便之后的解析。

然后显示出来。结果如下图所示，可以接收到`icmp`包（`TCP`的包也可以解析）

![image-20201017201525063](https://gitee.com/ethustdout/pics/raw/master/img/image-20201017201525063.png)



然后调用`spoof`函数进行欺骗（spoof函数并不是自带的）



## packet

由于`netinet/ip_icmp.h`中的格式不是很方便，自己写了头文件，放在`./net`目录下

虽然写的不是很好，但是能够使用



### 











<img src="../../../../Library/Application Support/typora-user-images/image-20201017230215415.png" alt="image-20201017230215415" style="zoom:200%;" />

