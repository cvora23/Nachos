#!/bin/tcsh

    if [ ! -d /home/scf-05/ptaskar/nachos-csci402/code/threads/terminal-logs ] 
    then
            mkdir /home/scf-05/ptaskar/nachos-csci402/code/threads/terminal-logs
    else
            script -f -q /home/scf-05/ptaskar/nachos-csci402/code/threads/terminal-logs/$$.log
    endif

gdb nachos <<GDB_INPUT
pwd
run nachos -P2 -d p
quit
   exit
   


GDB_INPUT