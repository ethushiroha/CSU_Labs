file = open("cipher.txt","r")
cipher = file.read()
alphabet = "abcdefghijklmnopqrstuvwxyz .,"

for i in range(1,29):
	for j in range(0,29):
		plain = ""
		for k in range(len(cipher)):
			if(cipher[k].islower()):
				plain += alphabet[((ord(cipher[k]) - ord('a')) * i + j) % 29]
			elif(cipher[k] == ' '):
				plain += alphabet[(26 * i + j) % 29]
			elif(cipher[k] == '.'):
				plain += alphabet[(27 * i + j) % 29]
			elif(cipher[k] == ','):
				plain += alphabet[(28 * i + j) % 29]
		if "the " in plain:
			print(plain)