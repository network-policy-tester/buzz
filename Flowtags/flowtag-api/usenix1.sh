#!/bin/sh

./flowtag_control fw clear
# Forwarding
./flowtag_control fw add -swId 0x100 -hostId 1 -connectorId 1
./flowtag_control fw add -swId 0x100 -hostId 2 -connectorId 2
./flowtag_control fw add -swId 0x100 -hostId 10 -connectorId 3
./flowtag_control fw add -swId 0x100 -hostId 11 -connectorId 3
./flowtag_control fw add -swId 0x100 -hostId 20 -connectorId 3


./flowtag_control fw add -swId 0x101 -hostId 1 -connectorId 1
./flowtag_control fw add -swId 0x101 -hostId 2 -connectorId 1
./flowtag_control fw add -swId 0x101 -hostId 10 -connectorId 1
./flowtag_control fw add -swId 0x101 -hostId 11 -connectorId 2
./flowtag_control fw add -swId 0x101 -hostId 20 -connectorId 3

./flowtag_control fw add -swId 0x200 -hostId 1 -connectorId 2
./flowtag_control fw add -swId 0x200 -hostId 2 -connectorId 2
./flowtag_control fw add -swId 0x200 -hostId 10 -connectorId 1
./flowtag_control fw add -swId 0x200 -hostId 11 -connectorId 3
./flowtag_control fw add -swId 0x200 -hostId 20 -connectorId 3


# MB Regist
./flowtag_control mb clear
# Host
./flowtag_control mb add -mbId 100 -hostId 1 -type CONSUME -address 10.0.0.1 -mask 255.255.0.0
./flowtag_control mb add -mbId 101 -hostId 2 -type CONSUME -address 10.1.0.1 -mask 255.255.0.0
./flowtag_control mb add -mbId 102 -hostId 11 -type CONSUME -address 172.16.1.1 -mask 255.255.255.0
./flowtag_control mb add -mbId 103 -hostId 20 -type CONSUME -address 192.168.0.0 -mask 255.255.255.0

# First NAT
./flowtag_control mb add -mbId 1 -hostId 10 -type NAT -address 20.0.0.1 -mask 255.255.255.0

# FW
./flowtag_control mb add -mbId 4 -hostId 11 -type NON_CONSUME



## Consume
#./flowtag_control tag add -tag 0 -next 3
./flowtag_control tag clear
./flowtag_control tag add -tag 12 -srcIP 10.0.0.1 -next 4
./flowtag_control tag add -tag 16 -srcIP 10.0.0.2 -next 4
./flowtag_control tag add -tag 20 -srcIP 10.1.0.1 -next 4

./flowtag_control tag add -tag 24 -srcIP 172.16.1.1 -next 3
./flowtag_control tag add -tag 28 -srcIP 172.16.1.1 -next -1


## Generate
./flowtag_control out clear

# For Nat
./flowtag_control out add -mid 1 -state 0 -preTag 12 -newTag 12 -next 4
./flowtag_control out add -mid 1 -state 0 -preTag 16 -newTag 16 -next 4
./flowtag_control out add -mid 1 -state 0 -preTag 20 -newTag 20 -next 4

# For LB
#./flowtag_control out add -mid 2 -state 0 -preTag 12 -newTag 20
#./flowtag_control out add -mid 2 -state 0 -preTag 16 -newTag 24

# For Squid
#./flowtag_control out add -mid 3 -state 0 -preTag 12 -newTag 20
#./flowtag_control out add -mid 3 -state 1 -preTag 12 -newTag 0
#./flowtag_control out add -mid 3 -state 1 -preTag 16 -newTag 28

# For FW (Nothing, don't generate any tag)


