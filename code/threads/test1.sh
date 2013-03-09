#!/bin/tcsh

set DIRECTORY = /home/scf-05/ptaskar/nachos-csci402/code/threads/terminal-logs

    if ( ! -d "$DIRECTORY" ) then
        echo "Directory does not exist.. So making a new directory $DIRECTORY"
        mkdir $DIRECTORY
    endif
    
#      script /home/scf-05/ptaskar/nachos-csci402/code/threads/terminal-logs/$$.log
      gdb nachos <<GDB_INPUT
      set logging on
      set logging file /home/scf-05/ptaskar/nachos-csci402/code/threads/terminal-logs/$$.log
      pwd
      run nachos -P2 -d p
      quit
      GDB_INPUT
      set logging off
      