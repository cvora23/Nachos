#!/bin/sh
gdb nachos <<GDB_INPUT
pwd
run nachos -P2 -d p
quit
GDB_INPUT
