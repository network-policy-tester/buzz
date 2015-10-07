#!/bin/bash

rm -f server
rm -f client

gcc server.c -o server
gcc flowtags_client.c -o client
