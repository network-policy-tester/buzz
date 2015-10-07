#!/bin/sh


#./flowtag_control tag add -tag 0 -next 3
./flowtag_control tag clear
./flowtag_control tag add -tag 12 -srcIP 10.0.0.1 -next 4
./flowtag_control tag add -tag 16 -srcIP 10.0.0.2 -next 4
./flowtag_control tag add -tag 20 -srcIP 10.1.0.1 -next 4


./flowtag_control tag add -tag 24 -srcIP 10.0.0.1 -next 5
./flowtag_control tag add -tag 28 -srcIP 10.0.0.2 -next 5
./flowtag_control tag add -tag 32 -srcIP 10.1.0.1 -next 5


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
./flowtag_control out add -mid 3 -state 0 -preTag 12 -newTag 20
./flowtag_control out add -mid 3 -state 1 -preTag 12 -newTag 0
./flowtag_control out add -mid 3 -state 1 -preTag 16 -newTag 28

# For FW (Nothing, don't generate any tag)



./flowtag_control mb clear

# NAT1
./flowtag_control mb add -mbid 1 -swid 10 -type NAT

# LB
./flowtag_control mb add -mbid 2 -swid 11 -type NON_CONSUME

# FW
#./flowtag_control mb add -mbid 4 -swid 11 -type NON_CONSUME
