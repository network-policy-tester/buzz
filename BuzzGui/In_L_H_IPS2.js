#Traffic
10.1.0.1 10.2.0.1
#Enforcement
LightIPS_1 bad_conn>=10 HeavyIPS_1
LightIPS_1 !(bad_conn>=10) Allow
HeavyIPS_1 bad_signature Block
HeavyIPS_1 !bad_signature Allow
#Customize
LightIPS_1:Threshold=10