clear
make
insmod hook.ko
ping 192.168.2.191 -c 4
rmmod hook.ko
dmesg -c