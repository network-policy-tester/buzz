#!/bin/sh

./flowtag_control mb clear
./flowtag_control mb add -mbid 1 -swid 10 -type NAT
./flowtag_control mb add -mbid 2 -swid 11 -type NAT

#./flowtag_control tag add -tag 0 -next 3
./flowtag_control tag clear
./flowtag_control tag add -tag 12 -srcIP 10.0.0.1 -next 4
./flowtag_control tag add -tag 16 -srcIP 10.0.0.2 -next 4
./flowtag_control tag add -tag 20 -srcIP 10.0.0.1 
./flowtag_control tag add -tag 24 -srcIP 10.0.0.2


./flowtag_control out clear
./flowtag_control out add -mid 1 -state 0 -preTag 12 -newTag 12
./flowtag_control out add -mid 1 -state 0 -preTag 16 -newTag 16
./flowtag_control out add -mid 2 -state 0 -preTag 12 -newTag 20
./flowtag_control out add -mid 2 -state 0 -preTag 16 -newTag 24
#./flowtag_control out add -mid 2 -state  167772161 -preTag 0 -newTag 20
#./flowtag_control out add -mid 2 -state  167837697 -preTag 0 -newTag 24
