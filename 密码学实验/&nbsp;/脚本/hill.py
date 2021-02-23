import numpy
import gmpy2

def ceil(a):
	if(a<0):
		return int(a-0.3)
	else:
		return int(a+0.3)

alphabet = 'abcdefghijklmnopqrstuvwxyz_,.'
pt = 'm..axtl.juyshoqpab,eswgq'
ct = 'tbredeuqrewuz_azv,cjurip'
flag = 'endjfovyrspgdnfnikod,thx'
dic = {}
dic_ = {}
index = 0
for i in alphabet:
	dic[i] = index
	dic_[index] = i
	index += 1
index = 0
pt_ = [0]*len(pt)
for i in pt:
	pt_[index] = dic[i]
	index += 1
index = 0
ct_ = [0]*len(ct)
for i in ct:
	ct_[index] = dic[i]
	index += 1
index = 0
flag_ = [0]*len(flag)
for i in flag:
	flag_[index] = dic[i]
	index += 1
n = 4
a = [[0 for i in range(n)] for i in range(n)]
b = [[0 for i in range(n)] for i in range(n)]
start = 0
for i in range(0,n):
	for j in range(0,n):
		a[i][j] = pt_[(i + start)*n+j]
for i in range(0,n):
	for j in range(0,n):
		b[i][j] = ct_[(i + start)*n+j]
print("b:",end="")
print(b)

det = ceil(numpy.linalg.det(b))
print(det)
_det = ceil(gmpy2.invert(det, 29)) % 29
print(_det)

B = [[0 for i in range(n)] for i in range(n)]
for i in range(n):
	for j in range(n):
		a1 = [[0 for i1 in range(n-1)] for i1 in range(n-1)]
		index_x = 0
		for i2 in range(0,n):
			if(i2==i):
				continue
			index_y = 0
			for j1 in range(0,n):
				if(j1==j):
					continue
				a1[index_x][index_y] = b[i2][j1]
				index_y += 1
			index_x += 1
		B[i][j] = ceil(numpy.linalg.det(a1)) * (-1)**(i+j) % 29 * _det % 29
print(B)

C = [[0 for i in range(n)] for i in range(n)]

for i in range(n):
	for j in range(n):
		for k in range(n):
			C[i][j] += a[k][j] * B[k][i]
			C[i][j] %= 29

print(C)
c = ""

for i in range(len(flag)//n):
	for j in range(n):
		sum_ = 0
		for k in range(n):
			sum_ += C[k][j] * flag_[i*n+k] % 29
		sum_ %= 29
		c += dic_[sum_]
print(c)

c = ""
for i in range(n):
	for j in range(n):
		c += dic_[B[j][i]]
print(c)

print(5650 * 11 % 29)