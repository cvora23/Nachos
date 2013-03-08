#!/bin/tcsh

$DIRECTORY=/home/scf-05/ptaskar/nachos-csci402/code/threads/terminal-logs
if if [! -d "$DIRECTORY" ]
  then
    echo "ABSENT"
    fi
else
    echo "absent"
fi