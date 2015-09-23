#Traffic
10.1.0.1 10.2.0.1
#Enforcement
CountingIPS_1 attack Block
CountingIPS_1 unknown SYNProxy
CountingIPS_1 benign Allow
SYNProxy tcp_completed Allow
SYNProxy !tcp_completed Block
#Customize
BohateiSYNIPS_1-Threshold=0