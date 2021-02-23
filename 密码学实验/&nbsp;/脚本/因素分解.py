"""
n = 8868033978299375939
p = 2211650759
q = 4009690021
e = 65537
c = 5651047833087707101

e * d = 1 mod (p-1)(q-1)
c = m ** e mod p * q
m = c ** d mod p * q
"""
import gmpy2
n = 14720903937642437179
p = 3624270589
q = 4061756311
e = 65537
c = 8564498610545887758
d = int(gmpy2.invert(e, (p-1) * (q-1))) #计算私钥
print(d)
m = pow(c, d, n) #解密
print(m)
print(hex(m))
