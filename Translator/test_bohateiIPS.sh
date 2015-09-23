#!/bin/bash
# SYN 
sendip -p ipv4 -is 10.2.0.1 -p tcp -ts r -td 1001
# SYN + ACK
wget http://10.2.0.1/index.html
