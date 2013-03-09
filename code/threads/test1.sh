#!/bin/tcsh

set DIRECTORY = /home/scf-05/ptaskar/nachos-csci402/code/threads/terminal-logs

    if ( ! -d "$DIRECTORY" ) then
        echo "Directory does not exist.. So making a new directory $DIRECTORY"
        mkdir $DIRECTORY
    endif
    
      #script /home/scf-05/ptaskar/nachos-csci402/code/threads/terminal-logs/$$.log
      gdb nachos <<GDB_INPUT
      pwd
      run nachos -P2 -d p
      quit
      exit
      GDB_INPUT
