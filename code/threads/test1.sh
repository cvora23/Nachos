#!/bin/tcsh

if [ -z "$UNDER_SCRIPT" ]; then
    if [ ! -d /home/scf-05/ptaskar/nachos-csci402/code/threads/terminal-logs ]; then
        mkdir /home/scf-05/ptaskar/nachos-csci402/code/threads/terminal-logs
    fi
    export UNDER_SCRIPT=1
    script -f -q /home/scf-05/ptaskar/nachos-csci402/code/threads/terminal-logs/$$.log


gdb nachos <<GDB_INPUT
pwd
run nachos -P2 -d p
quit
    exit
fi

GDB_INPUT
