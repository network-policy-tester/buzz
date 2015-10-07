#!/bin/sh

./flowtag_control mb clear

# First NAT
./flowtag_control mb add -mbid 1 -swid 10 -type NAT

# Squid
./flowtag_control mb add -mbid 3 -swid 11 -type NON_CONSUME

#./flowtag_control tag add -tag 0 -next 3
./flowtag_control tag clear
./flowtag_control tag add -tag 12 -srcIP 10.0.0.1 -next 4
./flowtag_control tag add -tag 16 -srcIP 10.0.0.2 -next 4
./flowtag_control tag add -tag 20 -srcIP 10.0.0.1 -next 3
./flowtag_control tag add -tag 24 -srcIP 172.16.1.1 -next 3
./flowtag_control tag add -tag 28 -srcIP 172.16.1.1 -next -1


./flowtag_control out clear
./flowtag_control out add -mid 1 -state 0 -preTag 12 -newTag 12
./flowtag_control out add -mid 1 -state 0 -preTag 16 -newTag 16
./flowtag_control out add -mid 2 -state 0 -preTag 12 -newTag 20
./flowtag_control out add -mid 2 -state 0 -preTag 16 -newTag 24

# For Squid
./flowtag_control out add -mid 3 -state 0 -preTag 12 -newTag 20
./flowtag_control out add -mid 3 -state 1 -preTag 12 -newTag 0
./flowtag_control out add -mid 3 -state 1 -preTag 16 -newTag 28
