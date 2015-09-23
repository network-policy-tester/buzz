#Traffic
10.1.0.1 10.2.0.1
#Enforcement
BohateiSYNIPS_1 DiffCount=SYNCount DROP
BohateiSYNIPS_1 DiffCount>Threshold BohateiSYNProxy
BohateiSYNIPS_1 DiffCount=Threshold PASS
#Customize
BohateiSYNIPS_1-Threshold=0