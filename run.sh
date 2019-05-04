#!/bin/bash
./server 39999 & 
sleep 1 & 
./client 127.0.0.1 39999 &
wait 
