import re

infile = open("multistage.adu", "r")
outfile = open("test.sh", "w")
ADUSeqSize = 4

class ADU:
	ADUname = ""
	ADUid = 0
	tcpSYN = 0
	tcpACK = 0
	sig = 0
	used = 0 

class cmd:
	cmdname = ""
	tcpSYN = 0
	tcpACK = 0
	seq = ""  
	content = ""
	used = 0
	ADUnum = 0
	signature = ""

ADUList = []
cmdList = []
scriptList = []

'''
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
'''

# input ADUs
for i in range(ADUSeqSize):
	ADUList.append(ADU())
	# print ADUList

for line in infile.readlines():
	if len(line) > 1:
		if re.search("pkt1.packet.tcpSYN", line):
			ADUList[0].ADUname = "tcpSYN"
			ADUList[0].tcpSYN = 1;
		if re.search("pkt1.packet.signature", line):
			ADUList[0].sig = 0;
		if re.search("pkt2.packet.tcpSYN", line):
			ADUList[1].ADUname = "tcpSYN"
			ADUList[1].tcpSYN = 1;
		if re.search("pkt2.packet.signature", line):
			ADUList[1].sig = 0;
		if re.search("pkt3.packet.tcpSYN", line):
			ADUList[2].ADUname = "tcpSYN"
			ADUList[2].tcpSYN = 1;
		if re.search("pkt3.packet.signature", line):
			ADUList[3].ADUname = "tcpSig"
			ADUList[3].sig = 1;


# input cmd
# 1) input wget
'''
curCmd = cmd()
curCmd.name = "wget"
curCmd.tcpSYN = 1
curCmd.tcpACK = 1
curCmd.seq = "tcpSYNtcpACK"
curCmd.content = "wget http://10.2.0.1/index.html"
curCmd.ADUnum = 2
cmdList.append(curCmd)
'''
# 2) input sendip ACK
curCmd = cmd()
curCmd.name = "sendIPPacket"
curCmd.tcpSYN = 1
curCmd.tcpACK = 0
curCmd.seq = "tcpSYN"
curCmd.content = "sendip -p ipv4 -is 10.1.0.1 -id 10.2.0.1 -p tcp -ts r -td 1002 -tfs 1"
curCmd.ADUnum = 1
cmdList.append(curCmd)
# 3) input sendip data packet bad signature
curCmd = cmd()
curCmd.name = "sendIPPacketSig"
curCmd.tcpSYN = 0
curCmd.tcpACK = 0
curCmd.seq = "tcpSig"
curCmd.content = "sendip -p ipv4 -is 10.1.0.1 -id 10.2.0.1 -p tcp -ts r -td 1002 -tfs 0 -f /buzz/packet/badpayload"
curCmd.ADUnum = 1
cmdList.append(curCmd)

# Heurestic translation
# sort the cmd from most ADUs to least ADUs
cmdList.sort(key=lambda x: x.ADUnum, reverse=True)
for cmdtmp in cmdList: # match cmd from most to least
	if cmdtmp.used == 0:
		# find all possible ADU seq start positions
		# j = 0
		for i in range(len(ADUList)-cmdtmp.ADUnum+1):
			print "i, range(len(ADUList)-cmdtmp.ADUnum+1)",i, range(len(ADUList)-cmdtmp.ADUnum+1)
			ADUstr = ""
			strlen = 0
			j = i
			# j = 0
			# find a seq of not used ADU with len = cmd seq
			print "len(ADUList), cmdtmp.ADUnum",len(ADUList), cmdtmp.ADUnum
			print " "
			while (j<len(ADUList)):
				print "ADUList[j].used", ADUList[j].used
				if ADUList[j].used == 0:
					ADUstr+=ADUList[j].ADUname
					strlen+=1
					if strlen == cmdtmp.ADUnum:
						break
				j+=1

			print "cmdtmp.seq, ADUstr:",cmdtmp.seq, ADUstr
			if re.match(cmdtmp.seq, ADUstr):
				print "cmd appended"
				scriptList.append(cmdtmp.content);
				# mark all matched AUDs as used
				tmpsize = j+1
				# while (j<len(ADUList)):
				while (j<tmpsize):
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
