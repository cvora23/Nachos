#!/bin/tcsh

set DIRECTORY = /home/scf-05/ptaskar/nachos-csci402/code/threads/terminal-logs

    if ( ! -d "$DIRECTORY" ) then
    echo "Directory does not exist.. So making a new directory $DIRECTORY"
    else
    echo "Directory does exist"
    endif
#            mkdir $DIRECTORY
#    else
#            script -f -q $DIRECTORY/$$.log
#    endif
#
#gdb nachos <<GDB_INPUT
#pwd
#run nachos -P2 -d p
#quit
#   exit
#
#GDB_INPUT
