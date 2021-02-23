file = open("cipher.txt",'r')
cipher = file.read()

for i in range(1,26):
	plain = ""
	for j in range(len(cipher)):
		if(cipher[j].islower()):
			plain += chr((ord(cipher[j]) - ord('a') + i) % 26 + ord('a'))
		elif(cipher[j].isupper()):
			plain += chr((ord(cipher[j]) - ord('A') + i) % 26 + ord('A'))
		else:
			plain += cipher[j]
	if "the" in plain:
		print(plain)
