import binascii

c = "402012"
c = bytes.fromhex(c).decode()
print(c)
all = 0
for i in c:
	all = ord(i) + all * 256
print(all)
#----------------

raw = "30466633346f59213b4139794520572b45514d61583151576638643a"

raw = list(bytes.fromhex(raw))
length = len(raw)
a = []
for i in range(length):
	a.append(chr(raw[i]^i))
print(a)
for i in range(length//2):
	a[2*i+1],a[2*i] = a[2*i],a[2*i+1]
for i in range(length):
	print(a[i],end="")