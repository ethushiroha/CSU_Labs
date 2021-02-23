file = open("cipher.txt","r")
cipher = file.read()
len_ = len(cipher)

for i in range(2,40):
	plain = ""
	x = len_ // i + 1
	y = len_ % i
	for j in range(0,x):
		for k in range(0,i):
			if(k < y):
				plain += cipher[j + k * x]
			elif(j == x-1):
				break
			else:
				plain += cipher[j + k * (x - 1) + y]
	print(plain[4])
	if plain[4] == "{":
		print(plain)
		print(i)
		exit()