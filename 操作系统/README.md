# CSU_OS
操作系统的实验



# 工具

是某大神写的一个一个开源的项目，包括`dram` 和 `fileview`，可以直接查看物理内存，很暴力

原项目的github地址找不到，从别的地方找到的源码，修改了源码和`Makefile`之后使得能够兼容我的系统



## 内核版本

```shell
shiroha@ubuntu:~/os$ uname -a
Linux ubuntu 4.15.0-45-generic #48~16.04.1-Ubuntu SMP Tue Jan 29 18:03:48 UTC 2019 x86_64 x86_64 x86_64 GNU/Linux
```



这里不多做介绍这个模块



# 内核驱动

用来获得进程的相关信息，便于查找页表等数据

由于页表、目录等数据都存放在内核态，所以使用的是内核模块编程



## Makefile

```makefile
obj-m:=task_struct.o                          #产生task_struct模块的目标文件
#目标文件  文件  要与模块名字相同
CURRENT_PATH:=$(shell pwd)             #模块所在的当前路径
LINUX_KERNEL:=$(shell uname -r)        #linux内核代码的当前版本
LINUX_KERNEL_PATH:=/usr/src/linux-headers-$(LINUX_KERNEL)

all:
	make -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) modules    #编译模块

clean:
	make -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) clean      #清理模块

```



## task_struct.c



### get_pgtable_macro

用来获取内核页表存放的地址，通过`read_cr3_pa()`函数实现



### vaddr2paddr

将进程的虚拟地址转换为实际地址

底下注释掉的是我想要修改的值，但是并不难修改这个地址，猜测可能是由于页面上锁保护机制



### print_pid

遍历进程目录，找到想要检索的程序

其中`p->pid == x`， 需要手动赋值。（之后可能会让他自动？）

`vaddr`，是该进程中地址，获取的是这个进程中这个地址的虚拟地址对物理地址的映射



# 操作过程

测试用例`hello.c`

1. 运行`./hello.c`

   ```shell
   shiroha@ubuntu:~/os$ ./hello 
   hello world
   0x4006a4
   ```

2. 新建终端`ps -a` 查看`hello` 的`pid`

   ```shell
   shiroha@ubuntu:~$ ps -a
      PID TTY          TIME CMD
     2182 pts/4    00:00:00 hello
     2207 pts/17   00:00:00 ps
   ```

3. 修改`p->pid == 2182`，`make`

   ```shell
   shiroha@ubuntu:~/os/task$ make
   make -C /usr/src/linux-headers-4.15.0-45-generic         M=/home/shiroha/os/task              modules    #编译模块
   make[1]: Entering directory '/usr/src/linux-headers-4.15.0-45-generic'
     CC [M]  /home/shiroha/os/task/task_struct.o
   ./scripts/ubuntu-retpoline-extract-one: line 263: /home/shiroha/os/task/task_struct.o.ur-safe: Permission denied
     Building modules, stage 2.
     MODPOST 1 modules
     CC      /home/shiroha/os/task/task_struct.mod.o
     LD [M]  /home/shiroha/os/task/task_struct.ko
   make[1]: Leaving directory '/usr/src/linux-headers-4.15.0-45-generic'
   ```

4. 加载驱动

   ```shell
   shiroha@ubuntu:~/os/x$ sudo insmod task_struct.ko
   [sudo] password for shiroha: 
   shiroha@ubuntu:~/os/x$ dmesg
   [ 1017.840343] Printf process'message begin:
   [ 1017.840444] 
   
   [ 1017.840445] total_vm:1127;
   [ 1017.840445] pid: 2182
   [ 1017.840446] cr0 = 0x80050033, cr3 = 0x2c3ca000
   [ 1017.840447] pgd_val = 0x800000003d8d2067, pgd_index = 0
   [ 1017.840448] p4d_val = 0x800000003d8d2067, p4d_index = 0
   [ 1017.840448] pud_val = 0x3d867067, pud_index = 0
   [ 1017.840449] pmd_val = 0x797b5067, pmd_index = 2
   [ 1017.840449] pte_val = 0x2cd28025, ptd_index = 0
   [ 1017.840450] page_addr = 2cd28000, page_offset = 6a4
   [ 1017.840450] vaddr = 4006a4, paddr = 2cd286a4
   ```

5. 使用`fileview`工具配合`dram`模块使用查看对应的物理内存是否正确

   ```shell
   000002CD286A0     6C6C656800020001        0A646C726F77206F    ....hello world.
   000002CD286B0     77206F6C6C656800        0A702500646C726F    .hello world.%p.
   ```

