#!/bin/sh


# MB Regist
./flowtag_control mb clear
# Host
./flowtag_control mb add -mbId 101 -hostId 1 -type CONSUME -address 10.0.0.1 -mask 255.255.0.0
./flowtag_control mb add -mbId 101 -hostId 1 -type CONSUME -address 192.168.0.1 -mask 255.255.255.0
./flowtag_control mb add -mbId 102 -hostId 2 -type CONSUME -address 10.1.0.1 -mask 255.255.0.0
./flowtag_control mb add -mbId 102 -hostId 2 -type CONSUME -address 192.168.1.1 -mask 255.255.255.0
./flowtag_control mb add -mbId 103 -hostId 3 -type CONSUME -address 10.2.0.1 -mask 255.255.0.0
./flowtag_control mb add -mbId 103 -hostId 3 -type CONSUME -address 192.168.2.1 -mask 255.255.255.0
./flowtag_control mb add -mbId 104 -hostId 4 -type CONSUME -address 10.3.0.1 -mask 255.255.0.0
./flowtag_control mb add -mbId 104 -hostId 4 -type CONSUME -address 192.168.3.1 -mask 255.255.255.0
./flowtag_control mb add -mbId 105 -hostId 5 -type CONSUME -address 10.4.0.1 -mask 255.255.0.0
./flowtag_control mb add -mbId 105 -hostId 5 -type CONSUME -address 192.168.4.1 -mask 255.255.255.0
./flowtag_control mb add -mbId 106 -hostId 6 -type CONSUME -address 10.5.0.1 -mask 255.255.0.0
./flowtag_control mb add -mbId 106 -hostId 6 -type CONSUME -address 192.168.5.1 -mask 255.255.255.0
./flowtag_control mb add -mbId 107 -hostId 7 -type CONSUME -address 10.6.0.1 -mask 255.255.0.0
./flowtag_control mb add -mbId 107 -hostId 7 -type CONSUME -address 192.168.6.1 -mask 255.255.255.0
./flowtag_control mb add -mbId 108 -hostId 8 -type CONSUME -address 10.7.0.1 -mask 255.255.0.0
./flowtag_control mb add -mbId 108 -hostId 8 -type CONSUME -address 192.168.7.1 -mask 255.255.255.0
./flowtag_control mb add -mbId 109 -hostId 9 -type CONSUME -address 10.8.0.1 -mask 255.255.0.0
./flowtag_control mb add -mbId 109 -hostId 9 -type CONSUME -address 192.168.8.1 -mask 255.255.255.0
./flowtag_control mb add -mbId 1010 -hostId 10 -type CONSUME -address 10.9.0.1 -mask 255.255.0.0
./flowtag_control mb add -mbId 1010 -hostId 10 -type CONSUME -address 192.168.9.1 -mask 255.255.255.0
./flowtag_control mb add -mbId 111 -hostId 11 -type CONSUME -address 172.16.1.1 -mask 255.255.255.0
./flowtag_control mb add -mbId 112 -hostId 12 -type CONSUME -address 172.16.2.1 -mask 255.255.255.0

# First NAT
./flowtag_control mb add -mbId 1 -hostId 10 -type NAT -address 20.0.0.1 -mask 255.255.255.0
./flowtag_control mb add -mbId 1 -hostId 10 -type NAT -address 30.0.0.1 -mask 255.255.255.0
./flowtag_control mb add -mbId 1 -hostId 10 -type NAT -address 40.0.0.1 -mask 255.255.255.0
./flowtag_control mb add -mbId 1 -hostId 10 -type NAT -address 50.0.0.1 -mask 255.255.255.0

# Second NAT
./flowtag_control mb add -mbId 2 -hostId 11 -type NAT -address 20.1.0.1 -mask 255.255.255.0
./flowtag_control mb add -mbId 2 -hostId 11 -type NAT -address 30.1.0.1 -mask 255.255.255.0
./flowtag_control mb add -mbId 2 -hostId 11 -type NAT -address 40.1.0.1 -mask 255.255.255.0
./flowtag_control mb add -mbId 2 -hostId 11 -type NAT -address 50.1.0.1 -mask 255.255.255.0


# FW
./flowtag_control mb add -mbId 3 -hostId 12 -type NON_CONSUME



## Consume
#./flowtag_control tag add -tag 0 -next 3
./flowtag_control tag clear
./flowtag_control tag add -tag 12 -srcIP 10.0.0.1 -next 2
./flowtag_control tag add -tag 16 -srcIP 10.1.0.1 -next 2
./flowtag_control tag add -tag 20 -srcIP 10.2.0.1 -next 2
./flowtag_control tag add -tag 24 -srcIP 10.3.0.1 -next 2
./flowtag_control tag add -tag 28 -srcIP 10.0.0.1 -next 2
./flowtag_control tag add -tag 32 -srcIP 10.1.0.1 -next 2
./flowtag_control tag add -tag 36 -srcIP 10.2.0.1 -next 2
./flowtag_control tag add -tag 40 -srcIP 10.3.0.1 -next 2

./flowtag_control tag add -tag 4 -srcIP 0.0.0.0 -next 1

## Generate
./flowtag_control out clear

# For Nat 1
./flowtag_control out add -mid 1 -state 0 -preTag 12 -newTag 12 -next 2
./flowtag_control out add -mid 1 -state 0 -preTag 16 -newTag 16 -next 2
./flowtag_control out add -mid 1 -state 0 -preTag 20 -newTag 16 -next 2
./flowtag_control out add -mid 1 -state 0 -preTag 24 -newTag 16 -next 2

# For Nat 2
./flowtag_control out add -mid 2 -state 0 -preTag 12 -newTag 28 -next 3
./flowtag_control out add -mid 2 -state 0 -preTag 16 -newTag 32 -next 3
./flowtag_control out add -mid 2 -state 0 -preTag 20 -newTag 36 -next 3
./flowtag_control out add -mid 2 -state 0 -preTag 24 -newTag 40 -next 3
#./flowtag_control out add -mid 1 -state 0 -preTag 20 -newTag 20 -next 2

# For FW
./flowtag_control out add -mid 3 -state 0 -preTag 28 -newTag 0 -next 0
./flowtag_control out add -mid 3 -state 0 -preTag 32 -newTag 0 -next 0
./flowtag_control out add -mid 3 -state 0 -preTag 36 -newTag 0 -next 0
./flowtag_control out add -mid 3 -state 0 -preTag 40 -newTag 0 -next 0

# For First NAT
./flowtag_control out add -mid 1 -state 0 -preTag 4 -newTag 4 -next 1

# For LB
#./flowtag_control out add -mid 2 -state 0 -preTag 12 -newTag 20
#./flowtag_control out add -mid 2 -state 0 -preTag 16 -newTag 24

# For Squid
#./flowtag_control out add -mid 3 -state 0 -preTag 12 -newTag 20
#./flowtag_control out add -mid 3 -state 1 -preTag 12 -newTag 0
#./flowtag_control out add -mid 3 -state 1 -preTag 16 -newTag 28

# For FW (Nothing, don't generate any tag)


