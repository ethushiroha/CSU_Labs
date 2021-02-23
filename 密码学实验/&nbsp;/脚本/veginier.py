def virginiaCrack(cipherText): # 解密函数
    length = getKeyLen(cipherText) #得到密钥长度
    key = getKey(cipherText,length) #找到密钥
    keyStr = ''
    for k in key:
        keyStr+=k
    print('the key:',keyStr)
    plainText = ''
    index = 0
    for ch in cipherText:
        if ord('a') <= ord(ch) <= ord('z'):
            c = chr((ord(ch)-ord(key[index%length])+26)%26+97)  #字母的ascii值与对该位置加密的密钥字母，相对位移
        elif ord('A') <= ord(ch) <= ord('Z'):
            c = chr((ord(ch)-ord(key[index%length])+26)%26+97)  #字母的ascii值与对该位置加密的密钥字母，相对位移
        else:
            c = ch
        plainText += c
        index+=1
    return plainText
def openfile(fileName): # 读文件
    file = open(fileName,'r',encoding="utf-8")
    text = file.read()
    file.close();
    text = text.replace('\n','')
    return text

# 统计次数IC值
def tongjicisu(s):
    tongjicisu = [0] * 26
    zff = ""
    ic=-0
    for t in s:
        if 65 <= ord(t) <= 90:
            zff += t
    for cisu in zff:
        tongjicisu[ord(cisu) - 65] += 1
    for i in range (len(tongjicisu)):
        xic=tongjicisu[i]*(tongjicisu[i]-1)/len(zff)/(len(zff)-1)
        ic+=xic
    return ic

def getKeyLen(s): # 获取密钥长度
    max_len = 27
    aves=[0]*max_len
    for i in range(1,max_len):
        count = 0
        zicuan=[]
        for t in range (i):
            fz=s[t:len(s):i]
            zicuan+=[fz]
            count+=1
        for js in range (i):
            zicuan[js]=zicuan[js].upper()
            ics=[0]*count
        #统计每个分组的IC值
        for r in range(count):
            ics[r]=tongjicisu(zicuan[r])
        ave =sum(ics)/count
        print('第{}次分组的IC值是{}'.format(i,ave))
        aves[i-1]=ave
        #找到最可能的密钥分组
        key_index=1
        max = 1
    for i in range(max_len):
        max1=abs(aves[i]-0.065)
        if max1<max:
            max = max1
            key_index=i+1
    print('key_length:',key_index)
    return key_index

def getKey(text,length): # 获取密钥
    key = [] # 定义空白列表用来存密钥
    alphaRate = [0.082, 0.015,0.028,0.043,0.127,
              0.022,0.02,0.061,0.07,0.002,0.008,
              0.04,0.024,0.067,0.075,0.019,0.001,
              0.06,0.063,0.091,0.028,0.01,0.023,0.001,0.02,0.001]
    matrix =textToList(text,length)   #将明文按照密钥长度分组成二位列表
    for i in range(length):
        w = [row[i] for row in matrix] #获取每组密文中第i位的密文    这都是用同一个字母加密的
        li = countList(w)    #计算里面的字母频率
        powLi = [] #交互重合指数
        for j in range(26):
            Sum = 0.00000
            for k in range(26):
                Sum += alphaRate[k]*li[k]  #   Ic的值Sum[k] += alphaRate[k]*li[k+j] 的话  就不用后面的切片操作
            powLi.append(Sum)
            li = li[1:]+li[:1]#循环移位  向左移一位
        Abs = 1
        ch = ''
        for j in range(len(powLi)):
             if abs(powLi[j] -0.065546)<Abs: # 找出最接近英文字母重合指数的项
                 Abs = abs(powLi[j] -0.065546) # 保存最接近的距离，作为下次比较的基准
                 ch = chr(j+97)
        key.append(ch)
    return key

def countList(lis): # 统计字母频率
    li = []
    alphabet = [chr(i) for i in range(97,123)]
    for c in alphabet:
        count = 0
        for ch in lis:
            if ch == c:
                count+=1
        li.append(count/len(lis))
    return li        #返回字母表每个字母的出现概率

def textToList(text,length): # 根据密钥长度将密文分组
    textMatrix = []   #二维表  里面添加分组数个分组  每个分组中都是按照密钥加密的
    row = []     #行  = 分组   每个row都是按照密钥加密的   长度为密钥的元素个数
    index = 0    #从0开始
    for ch in text:
        row.append(ch)
        index += 1
        if index % length ==0:
            textMatrix.append(row)
            row = []
    return textMatrix

if __name__ == '__main__':
    cipherText = openfile(r'cipher.txt') # 这里要根据文档目录的不同而改变
    plainText= virginiaCrack(cipherText.lower())
    print('the plainText:\n',plainText)
