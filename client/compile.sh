#!/bin/bash

#Manavai TEIKITUHAAHAA - Lorrain BALBIANI
echo "##############################"
echo "Compilation en version 32 bits"
echo "##############################"

gcc -m32 "mainServer.c"  -o "server32.exe" -Wall -pipe

echo "##############################"
echo "Compilation en version 64 bits"
echo "##############################"

gcc -m64 "mainServer.c"  -o "server64.exe" -Wall -pipe

./server64.exe  -debug