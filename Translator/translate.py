import re

infile = open("input_ADUs", "r")
outfile = open("test.sh", "w")

class ADU:
	ADUname = ""
	ADUid = 0
	tcpSYN = 0
	tcpACK = 0
	used = 0 

class cmd:
	cmdname = ""
	tcpSYN = 0
	tcpACK = 0
	seq = ""  
	content = ""
	used = 0
	ADUnum = 0

ADUList = []
cmdList = []
scriptList = []

# input ADUs
curADU = ADU()
for line in infile.readlines():
	# print line
	if len(line) > 1:
		# print line
		if re.search("locatedPacket", line):
			curADU = ADU()
		if re.search("tcpACK", line):
			elems = line.split()
			curADU.tcpACK = int(elems[2])
			if curADU.tcpACK == 1:
				curADU.ADUname = "tcpACK"
		if re.search("tcpSYN", line):
			elems = line.split()
			curADU.tcpSYN = int(elems[2])
			ADUList.append(curADU)
			if curADU.tcpSYN == 1:
				curADU.ADUname = "tcpSYN"
		#elems = line.split()
		#print elems[0]

# input cmd
# 1) input wget
curCmd = cmd()
curCmd.name = "wget"
curCmd.tcpSYN = 1
curCmd.tcpACK = 1
curCmd.seq = "tcpSYNtcpACK"
curCmd.content = "wget http://10.2.0.1/index.html"
curCmd.ADUnum = 2
cmdList.append(curCmd)
# 2) input sendip
curCmd = cmd()
curCmd.name = "sendIPPacket"
curCmd.tcpSYN = 1
curCmd.tcpACK = 0
curCmd.seq = "tcpSYN"
curCmd.content = "sendip -p ipv4 -is 10.2.0.1 -p tcp -ts r -td 1001"
curCmd.ADUnum = 1
cmdList.append(curCmd)

# Heurestic translation
# sort the cmd from most ADUs to least ADUs
cmdList.sort(key=lambda x: x.ADUnum, reverse=True)
for cmdtmp in cmdList: # match cmd from most to least
	if cmdtmp.used == 0:
		# find all possible ADU seq start positions
		for i in range(len(ADUList)-cmdtmp.ADUnum+1):
			# print i, range(len(ADUList)-cmdtmp.ADUnum+1)
			ADUstr = ""
			strlen = 0
			j = i
			# find a seq of not used ADU with len = cmd seq
			while (j<len(ADUList)):
				if ADUList[j].used == 0:
					ADUstr+=ADUList[j].ADUname
					strlen+=1
					if strlen == cmdtmp.ADUnum:
						break
				j+=1

			print cmdtmp.seq, ADUstr
			if re.match(cmdtmp.seq, ADUstr):
				scriptList.append(cmdtmp.content);
				# mark all matched AUDs as used
				while (j<len(ADUList)):
					if ADUList[j].used == 0:
						#ADUstr+=ADUList[j].ADUname
						ADUList[j].used = 1
						strlen+=1
						if strlen == cmdtmp.ADUnum:
							break
					j+=1
	cmdtmp.used = 1

# output test script
outfile.write("#!/bin/bash")
outfile.write("\n")	
for linetmp in scriptList:
	outfile.write(linetmp)
	outfile.write("\n")	

# cleanup
infile.close()
outfile.close()