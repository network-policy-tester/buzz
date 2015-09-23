#Traffic
192.168.123.1 192.168.123.2 
#Enforcement
IPS_1 Cond1 FW_1
IPS_1 Cond2 Proxy_1
Proxy_1 Cond3 FW_2
#Customize
IPS_1SYN_RECEIVED SYN_RECEIVED>10