
data = list(raw_input("input data: "))
hexd = ""

for i in data:
    hexd += "\\x" + i.encode('hex')
    #print hexd
print hexd
