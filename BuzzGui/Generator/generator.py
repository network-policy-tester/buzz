import re

in_driver = open("multistage_driver.c", "r") 
in_utils = open("multistage_utils.h", "r") 
in_policy = open("policy_multistage.js", "r") 
out_driver = open("driver.c", "w")
out_utils = open("utils.h", "w")

for line in in_policy.readlines():
	m = re.match("LightIPS_1:Threshold=", line)
	if m:
		threshold = line.split('=')[1]
		# print threshold


for line in in_utils.readlines():
	m = re.match("#define  DIFF_THRESHOLD", line)
	if m:
		out_utils.write("#define  DIFF_THRESHOLD "+threshold+"\n")	
	else:	
		out_utils.write(line)

for line in in_driver.readlines():
	out_driver.write(line)

# cleanup
in_policy.close()
in_utils.close()
out_utils.close()
in_driver.close()
out_driver.close()