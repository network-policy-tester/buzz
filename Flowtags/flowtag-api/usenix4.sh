#!/bin/sh

# Forwarding
./flowtag_control fw clear
./flowtag_control fw add -swId 0x100 -hostId 1 -connectorId 1
./flowtag_control fw add -swId 0x100 -hostId 2 -connectorId 2
./flowtag_control fw add -swId 0x100 -hostId 10 -connectorId 3
./flowtag_control fw add -swId 0x100 -hostId 11 -connectorId 4
./flowtag_control fw add -swId 0x100 -hostId 20 -connectorId 4


./flowtag_control fw add -swId 0x101 -hostId 1 -connectorId 1
./flowtag_control fw add -swId 0x101 -hostId 2 -connectorId 1
./flowtag_control fw add -swId 0x101 -hostId 10 -connectorId 1
./flowtag_control fw add -swId 0x101 -hostId 11 -connectorId 2
./flowtag_control fw add -swId 0x101 -hostId 20 -connectorId 3


# MB Regist
./flowtag_control mb clear
# Host
./flowtag_control mb add -mbId 100 -hostId 1 -type CONSUME -address 10.0.0.1 -mask 255.255.0.0
./flowtag_control mb add -mbId 101 -hostId 2 -type CONSUME -address 10.1.0.1 -mask 255.255.0.0
./flowtag_control mb add -mbId 102 -hostId 10 -type CONSUME -address 172.16.0.1 -mask 255.255.255.0
./flowtag_control mb add -mbId 102 -hostId 11 -type CONSUME -address 172.16.1.1 -mask 255.255.255.0
./flowtag_control mb add -mbId 103 -hostId 20 -type CONSUME -address 192.168.0.0 -mask 255.255.255.0

# Proxy
./flowtag_control mb add -mbId 1 -hostId 10 -type NON_CONSUME

# IDS
./flowtag_control mb add -mbId 2 -hostId 11 -type NON_CONSUME


#./flowtag_control tag add -tag 0 -next 3
./flowtag_control tag clear
./flowtag_control tag add -tag 12 -srcIP 10.0.0.1 -next 4
./flowtag_control tag add -tag 16 -srcIP 10.0.0.2 -next 4
./flowtag_control tag add -tag 20 -srcIP 10.1.0.1 -next 4


./flowtag_control tag add -tag 24 -srcIP 10.0.0.1 -next 5
./flowtag_control tag add -tag 28 -srcIP 10.0.0.2 -next 5
./flowtag_control tag add -tag 32 -srcIP 10.1.0.1 -next 5

#./flowtag_control tag add -tag 36 -srcIP 172.16.0.1  -next 4
#./flowtag_control tag add -tag 40 -srcIP 172.16.0.1  -next 1
#./flowtag_control tag add -tag 44 -srcIP 172.16.0.1  -next 2
./flowtag_control tag add -tag 36 -srcIP 192.168.0.1   -next 4
./flowtag_control tag add -tag 40 -srcIP 192.168.0.1   -next 1
./flowtag_control tag add -tag 44 -srcIP 192.168.0.1   -next 2


./flowtag_control out clear
# For Nat
./flowtag_control out add -mid 1 -state 0 -preTag 12 -newTag 12
./flowtag_control out add -mid 1 -state 0 -preTag 16 -newTag 16
./flowtag_control out add -mid 1 -state 0 -preTag 20 -newTag 20

# For LB
./flowtag_control out add -mid 2 -state 0 -preTag 12 -newTag 24
./flowtag_control out add -mid 2 -state 0 -preTag 16 -newTag 28
./flowtag_control out add -mid 2 -state 0 -preTag 20 -newTag 32

# For Squid
./flowtag_control out add -mid 3 -state 0 -preTag 12 -newTag 36 -next 2
./flowtag_control out add -mid 3 -state 1 -preTag 12 -newTag 36 -next 2
./flowtag_control out add -mid 3 -state 0 -preTag 16 -newTag 40 -next 100
./flowtag_control out add -mid 3 -state 1 -preTag 16 -newTag 40 -next 100
./flowtag_control out add -mid 3 -state 0 -preTag 20 -newTag 44 -next 101
./flowtag_control out add -mid 3 -state 1 -preTag 20 -newTag 44 -next 101

# For FW (Nothing, don't generate any tag)
